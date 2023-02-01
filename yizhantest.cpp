#include <iostream>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include "png.h"
#include "jpeglib.h"
#include "jerror.h"
#include "util.cpp"

using std::string;

struct Image {
  uint32_t width;
  uint32_t height;
  uint8_t **data;
};
void mallocImageData(Image& img, int width, int height) {
  img.data = (uint8_t **)malloc(img.height*sizeof(png_bytep));
  for(int y = 0; y < img.height; y++) {
    img.data[y] = (uint8_t*)malloc(img.width*sizeof(png_bytep));
  }
}
void showImageData(Image& img) {
  std::cout << "showImagePixelData:" << std::endl;
  for (int i = 0; i < img.height; i++) {
    for (int j = 0; j < img.width * 4; j++) {
      std::cout << unsigned(img.data[i][j]) << " ";
    }
    std::cout << std::endl;
  }
}

struct ImageReadHost {
  int position;
  uint8_t *data;
};
void decodeImage(Image& img, string filepath);
void decodePngBuffer(Image& img, uint8_t *buffer, size_t size);
void decodeJpegBuffer(Image& img, uint8_t *buffer, size_t size);

void encodeImage(Image& img);

void writeBlankPng(int width, int height);

void write_JPEG_file()
{
  int quality = 75;
  int data_precision = 8;
  int WIDTH = 2;
  int HEIGHT = 2;
  /* This struct contains the JPEG compression parameters and pointers to
   * working space (which is allocated as needed by the JPEG library).
   * It is possible to have several such structures, representing multiple
   * compression/decompression processes, in existence at once.  We refer
   * to any one struct (and its associated working data) as a "JPEG object".
   */
  struct jpeg_compress_struct cinfo;
  /* This struct represents a JPEG error handler.  It is declared separately
   * because applications often want to supply a specialized error handler
   * (see the second half of this file for an example).  But here we just
   * take the easy way out and use the standard error handler, which will
   * print a message on stderr and call exit() if compression fails.
   * Note that this struct must live as long as the main JPEG parameter
   * struct, to avoid dangling-pointer problems.
   */
  struct jpeg_error_mgr jerr;
  /* More stuff */
  FILE *outfile;                /* target file */
  JSAMPARRAY image_buffer = NULL;
                                /* Points to large array of R,G,B-order data */
  JSAMPROW row_pointer[1];      /* pointer to JSAMPLE row[s] */
  J12SAMPARRAY image_buffer12 = NULL;
                                /* Points to large array of R,G,B-order 12-bit
                                   data */
  J12SAMPROW row_pointer12[1];  /* pointer to J12SAMPLE row[s] */
  int row_stride;               /* physical row width in image buffer */
  int row, col;

  /* Step 1: allocate and initialize JPEG compression object */

  /* We have to set up the error handler first, in case the initialization
   * step fails.  (Unlikely, but it could happen if you are out of memory.)
   * This routine fills in the contents of struct jerr, and returns jerr's
   * address which we place into the link field in cinfo.
   */
  cinfo.err = jpeg_std_error(&jerr);
  /* Now we can initialize the JPEG compression object. */
  jpeg_create_compress(&cinfo);

  /* Step 2: specify data destination (eg, a file) */
  /* Note: steps 2 and 3 can be done in either order. */

  /* Here we use the library-supplied code to send compressed data to a
   * stdio stream.  You can also write your own code to do something else.
   * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
   * requires it in order to write binary files.
   */
  if ((outfile = fopen("test.jpeg", "wb")) == NULL)
    ERREXIT(&cinfo, JERR_FILE_WRITE);
  jpeg_stdio_dest(&cinfo, outfile);

  /* Step 3: set parameters for compression */

  /* First we supply a description of the input image.
   * Four fields of the cinfo struct must be filled in:
   */
  cinfo.image_width = WIDTH;            /* image width and height, in pixels */
  cinfo.image_height = HEIGHT;
  cinfo.input_components = 3;           /* # of color components per pixel */
  cinfo.in_color_space = JCS_RGB;       /* colorspace of input image */
  cinfo.data_precision = data_precision; /* data precision of input image */
  /* Now use the library's routine to set default compression parameters.
   * (You must set at least cinfo.in_color_space before calling this,
   * since the defaults depend on the source color space.)
   */
  jpeg_set_defaults(&cinfo);
  /* Now you can set any non-default parameters you wish to.
   * Here we just illustrate the use of quality (quantization table) scaling:
   */
  jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);
  /* Use 4:4:4 subsampling (default is 4:2:0) */
  cinfo.comp_info[0].h_samp_factor = cinfo.comp_info[0].v_samp_factor = 1;

  /* Step 4: Start compressor */

  /* TRUE ensures that we will write a complete interchange-JPEG file.
   * Pass TRUE unless you are very sure of what you're doing.
   */
  jpeg_start_compress(&cinfo, TRUE);

  /* Step 5: allocate and initialize image buffer */

  row_stride = WIDTH * 3;       /* J[12]SAMPLEs per row in image_buffer */
  /* Make a sample array that will go away when done with image.  Note that,
   * for the purposes of this example, we could also create a one-row-high
   * sample array and initialize it for each successive scanline written in the
   * scanline loop below.
   */
  if (cinfo.data_precision == 12) {
    image_buffer12 = (J12SAMPARRAY)(*cinfo.mem->alloc_sarray)
      ((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, HEIGHT);

    /* Initialize image buffer with a repeating pattern */
    for (row = 0; row < HEIGHT; row++) {
      for (col = 0; col < WIDTH; col++) {
        image_buffer12[row][col * 3] =
          (col * (MAXJ12SAMPLE + 1) / WIDTH) % (MAXJ12SAMPLE + 1);
        image_buffer12[row][col * 3 + 1] =
          (row * (MAXJ12SAMPLE + 1) / HEIGHT) % (MAXJ12SAMPLE + 1);
        image_buffer12[row][col * 3 + 2] =
          (row * (MAXJ12SAMPLE + 1) / HEIGHT +
           col * (MAXJ12SAMPLE + 1) / WIDTH) % (MAXJ12SAMPLE + 1);
      }
    }
  } else {
    image_buffer = (*cinfo.mem->alloc_sarray)
      ((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, HEIGHT);

    for (row = 0; row < HEIGHT; row++) {
      for (col = 0; col < WIDTH; col++) {
        image_buffer[row][col * 3] =
          (col * (MAXJSAMPLE + 1) / WIDTH) % (MAXJSAMPLE + 1);
        image_buffer[row][col * 3 + 1] =
          (row * (MAXJSAMPLE + 1) / HEIGHT) % (MAXJSAMPLE + 1);
        image_buffer[row][col * 3 + 2] =
          (row * (MAXJSAMPLE + 1) / HEIGHT + col * (MAXJSAMPLE + 1) / WIDTH) %
          (MAXJSAMPLE + 1);
      }
    }
  }

  /* Step 6: while (scan lines remain to be written) */
  /*           jpeg_write_scanlines(...); */

  /* Here we use the library's state variable cinfo.next_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   * To keep things simple, we pass one scanline per call; you can pass
   * more if you wish, though.
   */
  if (cinfo.data_precision == 12) {
    while (cinfo.next_scanline < cinfo.image_height) {
      /* jpeg12_write_scanlines expects an array of pointers to scanlines.
       * Here the array is only one element long, but you could pass
       * more than one scanline at a time if that's more convenient.
       */
      row_pointer12[0] = image_buffer12[cinfo.next_scanline];
      (void)jpeg12_write_scanlines(&cinfo, row_pointer12, 1);
    }
  } else {
    while (cinfo.next_scanline < cinfo.image_height) {
      /* jpeg_write_scanlines expects an array of pointers to scanlines.
       * Here the array is only one element long, but you could pass
       * more than one scanline at a time if that's more convenient.
       */
      row_pointer[0] = image_buffer[cinfo.next_scanline];
      (void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
  }

  /* Step 7: Finish compression */

  jpeg_finish_compress(&cinfo);
  /* After finish_compress, we can close the output file. */
  fclose(outfile);

  /* Step 8: release JPEG compression object */

  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_compress(&cinfo);

  /* And we're done! */
}

int main() {
  string filePath = "test.png";
  std::cout << "jpng by yizhanzhang: " << filePath << std::endl;
  Image img;
  decodeImage(img, filePath);
  showImageData(img);
  encodeImage(img);
  return 0;
}

void writeBlankPng(int width, int height) {
  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (setjmp(png_jmpbuf(png_ptr))) {
    throw std::invalid_argument("png_ptr jmp buf");
  }

  FILE *fp = fopen("blank.png", "wb");
  png_init_io(png_ptr, fp);

  png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
  png_write_info(png_ptr, info_ptr);

  for (int y = 0; y < height; y++) {
    int length = width * 4;
    png_byte bytes[length];
    memset(bytes, 128, length);
    png_write_row(png_ptr, bytes);
  }
  png_write_end(png_ptr, info_ptr);

  png_destroy_write_struct(&png_ptr, &info_ptr);
  fclose(fp);
}

void decodeImage(Image& img, string filepath) {
  struct stat statbuf;
  if (stat(filepath.c_str(), &statbuf) != 0) {
    throw std::invalid_argument("open file error:" + filepath);
  };
  size_t filesize = statbuf.st_size;

  FILE *fp = fopen((char *)filepath.c_str(), "rb");
  if (!fp) {
    throw std::invalid_argument("open file error:" + filepath);
  }
  uint8_t buffer[filesize];
  fread(buffer, filesize, 1, fp);
  fclose(fp);
  
  if (endsWith(filepath, ".png")) {
    decodePngBuffer(img, buffer, filesize);
  } else if(endsWith(filepath, ".jpg") || endsWith(filepath, ".jpeg")) {
    decodeJpegBuffer(img, buffer, filesize);
  } else {
    throw std::invalid_argument("unsupport file format");
  }
}

void read_png_data_fn(png_structp png_ptr, png_bytep data, size_t length) {
  ImageReadHost *imgHost = (ImageReadHost *)png_get_io_ptr(png_ptr);
  memcpy(data, &(imgHost->data[imgHost->position]), length);
  imgHost->position += length;
}
void decodePngBuffer(Image& img, uint8_t *buffer, size_t size) {
  if(png_sig_cmp(buffer, 0, 4)) {
    throw std::invalid_argument("this is not png");
  }

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  png_infop info_ptr = png_create_info_struct(png_ptr);

  ImageReadHost *imgHost = (ImageReadHost *)malloc(sizeof(ImageReadHost));
  imgHost->position = 0;
  imgHost->data = buffer;
  png_set_read_fn(png_ptr, (void *)imgHost, read_png_data_fn);
  png_read_info(png_ptr, info_ptr);
  png_get_IHDR(png_ptr, info_ptr, &img.width, &img.height, NULL, NULL, NULL, NULL, NULL);
  mallocImageData(img, img.width, img.height);
  png_read_image(png_ptr, img.data);
  png_read_end(png_ptr, info_ptr);
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  free(imgHost);
}

void decodeJpegBuffer(Image& img, uint8_t *buffer, size_t size) {
  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  png_infop info_ptr = png_create_info_struct(png_ptr);

  ImageReadHost *imgHost = (ImageReadHost *)malloc(sizeof(ImageReadHost));
  imgHost->position = 0;
  imgHost->data = buffer;
  png_set_read_fn(png_ptr, (void *)imgHost, read_png_data_fn);
  png_read_info(png_ptr, info_ptr);
  png_get_IHDR(png_ptr, info_ptr, &img.width, &img.height, NULL, NULL, NULL, NULL, NULL);
  img.data = (uint8_t **)malloc(img.height * 4 * sizeof(png_bytep));
  png_read_image(png_ptr, img.data);
  png_read_end(png_ptr, info_ptr);
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  free(imgHost);
}

void encodeImage(Image& img) {

}
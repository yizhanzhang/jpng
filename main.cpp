#include "main.h"
#include "util.cpp"
#include "png.cpp"

using std::string;

Image::Image() {
  data = NULL;
  buffer = NULL;
}
Image::~Image() {
  freeBuffer();
  freeImageData();
}
void Image::mallocImageData() {
  freeImageData();
  data = (uint8_t **)malloc(height*sizeof(png_bytep));
  for(int i = 0; i < height; i++) {
    data[i] = (uint8_t*)malloc(width*sizeof(png_bytep));
  }
}
void Image::freeImageData() {
  if (data == NULL) return;
  for (int i = 0; i < height; i++) {
    free(data[i]);
  }
  free(data);
}
void Image::freeBuffer() {
  if (data == NULL) return;
  free(buffer);
}
void Image::showImageData() {
  std::cout << "showImagePixelData:" << std::endl;
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width * 4; j++) {
      std::cout << unsigned(data[i][j]) << " ";
    }
    std::cout << std::endl;
  }
}

void decodeImage(Image& img, string filepath);
void decodeJpegBuffer(Image& img, uint8_t *buffer, size_t size);
void encodeImage(Image& img, string filepath);

int main() {
  std::cout << "jpng by yizhanzhang: " << filePath << std::endl;

  string filePath = "test.png";
  string outpath = "test3.png";

  Image img;
  decodeImage(img, filePath);
  img.showImageData();
  encodeImage(img, outpath);

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
    throw std::invalid_argument("stat file error:" + filepath);
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

void decodeJpegBuffer(Image& img, uint8_t *buffer, size_t size) {
  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  png_infop info_ptr = png_create_info_struct(png_ptr);

  ImageIOHost *imgHost = (ImageIOHost *)malloc(sizeof(ImageIOHost));
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

void encodeImage(Image& img, string filepath) {
  struct stat statbuf;
  if (stat(filepath.c_str(), &statbuf) == 0) {
    throw std::invalid_argument("stat file already existed:" + filepath);
  };

  if (endsWith(filepath, ".png")) {
    encodePngBuffer(img);
  } else if(endsWith(filepath, ".jpg") || endsWith(filepath, ".jpeg")) {
    // decodeJpegBuffer(img, buffer, filesize);
  } else {
    throw std::invalid_argument("unsupport file format");
  }

  FILE *fp = fopen((char *)filepath.c_str(), "wb");
  if (!fp) {
    throw std::invalid_argument("open file error:" + filepath);
  }
  fwrite(img.buffer, img.bufferSize, 1, fp);
  fclose(fp);
}
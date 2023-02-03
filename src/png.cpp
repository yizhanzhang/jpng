#include "png.h"

struct ImageIOHost {
  int position;
  uint8_t *data;
};

void read_png_data_fn(png_structp png_ptr, png_bytep data, size_t length) {
  ImageIOHost *imgHost = (ImageIOHost *)png_get_io_ptr(png_ptr);
  memcpy(data, &(imgHost->data[imgHost->position]), length);
  imgHost->position += length;
}
void decodePngBuffer(Image& img, uint8_t *buffer, size_t size) {
  if(png_sig_cmp(buffer, 0, 4)) {
    napi_throw_error(img._env, NULL, "this is not png");
    return void(0);
  }

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  png_infop info_ptr = png_create_info_struct(png_ptr);

  ImageIOHost imgHost;
  imgHost.position = 0;
  imgHost.data = buffer;
  png_set_read_fn(png_ptr, (void *)&imgHost, read_png_data_fn);
  png_read_info(png_ptr, info_ptr);
  int bit_depth, color_type;
  png_get_IHDR(png_ptr, info_ptr, &img.width, &img.height, &bit_depth, &color_type, NULL, NULL, NULL);
  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    png_set_expand(png_ptr);
  } else if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
    png_set_gray_to_rgb(png_ptr);
  }
  if (color_type != PNG_COLOR_TYPE_RGB_ALPHA && color_type != PNG_COLOR_TYPE_GRAY_ALPHA) {
    png_set_add_alpha(png_ptr, 255, PNG_FILTER_NONE);
  }
  if (bit_depth < 8) {
    png_set_packing(png_ptr);
  } else if (bit_depth == 16) {
    png_set_strip_16(png_ptr);
  }
  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
    png_set_tRNS_to_alpha(png_ptr);
  }
  png_read_update_info(png_ptr, info_ptr);
  bit_depth = png_get_bit_depth(png_ptr, info_ptr);
  color_type = png_get_color_type(png_ptr, info_ptr);

  img.mallocImageData();
  png_read_image(png_ptr, img.data);
  png_read_end(png_ptr, info_ptr);
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
}

void write_date_fn(png_structp png_ptr, png_bytep data, size_t length) {
  ImageIOHost *imgHost = (ImageIOHost *)png_get_io_ptr(png_ptr);
  size_t wholeLength = imgHost->position + length;
  imgHost->data = (uint8_t *)realloc(imgHost->data, wholeLength);
  memcpy(&imgHost->data[imgHost->position], data, length);
  imgHost->position += length;
}
void encodePngBuffer(Image& img) {
  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  png_infop info_ptr = png_create_info_struct(png_ptr);

  ImageIOHost imgHost;
  imgHost.position = 0;
  imgHost.data = (uint8_t *)malloc(0);
  png_set_write_fn(png_ptr, (void *)&imgHost, write_date_fn, NULL);
  png_set_IHDR(png_ptr, info_ptr, img.width, img.height, 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
  png_write_info(png_ptr, info_ptr);
  png_write_image(png_ptr, img.data);
  png_write_end(png_ptr, info_ptr);
  img.buffer = imgHost.data;
  img.bufferSize = imgHost.position;

  png_destroy_write_struct(&png_ptr, &info_ptr);
}

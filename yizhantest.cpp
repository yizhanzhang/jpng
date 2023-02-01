#include <iostream>
#include <stdio.h>
#include "png.h"

void writeBlankPng(int width, int height);

int main() {
  std::cout << "here is yizhantest" << std::endl;
  writeBlankPng(2, 2);
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
};

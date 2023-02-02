#include "main.h"
#include "util.cpp"
#include "png.cpp"
#include "jpeg.cpp"

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
    data[i] = (uint8_t*)malloc(4*width*sizeof(png_bytep));
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
void encodeImage(Image& img, string filepath);

int main() {
  std::cout << "jpng by yizhanzhang: " << std::endl;

  string filePath = "ptest.png";
  string outpath = "test3.jpeg";

  Image img;
  decodeImage(img, filePath);
  // img.showImageData();
  encodeImage(img, outpath);

  return 0;
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

void encodeImage(Image& img, string filepath) {
  struct stat statbuf;
  if (stat(filepath.c_str(), &statbuf) == 0) {
    // throw std::invalid_argument("stat file already existed:" + filepath);
  };

  if (endsWith(filepath, ".png")) {
    encodePngBuffer(img);
  } else if(endsWith(filepath, ".jpg") || endsWith(filepath, ".jpeg")) {
    encodeJpegBuffer(img);
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
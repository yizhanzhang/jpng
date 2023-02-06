#include "image.h"
#include "png.cpp"
#include "jpeg.cpp"

Image::Image() {
  data = NULL;
};

Image::~Image() {
  freeImageData();
};

Result Image::decodeImage(CompressData& inputData) {
  Result result;

  Result (*decodeFunc)(Image &img, CompressData& inputData);

  if (inputData.type == "png") {
    decodeFunc = decodePngBuffer;
  } else if(inputData.type == "jpg"|| inputData.type == "jpeg") {
    decodeFunc = decodeJpegBuffer;
  } else {
    result.setError("unsupport file format:" + inputData.type);
    return result;
  }

  result = decodeFunc(*this, inputData);
  return result;
};

Result Image::encodeImage(CompressData& outputData) {
  Result result;
  Result (*encodeFunc)(Image &img, CompressData& outputData);
  if (outputData.type == "png") {
    encodeFunc = encodePngBuffer;
  } else if(outputData.type == "jpg"|| outputData.type == "jpeg") {
    encodeFunc = encodeJpegBuffer;
  } else {
    result.setError("unsupport file format:" + outputData.type);
    return result;
  }

  result = encodeFunc(*this, outputData);
  if(result.flag == -1) {
    return result;
  }

  return result;
};


void Image::mallocImageData() {
  freeImageData();
  data = (uint8_t **)malloc(height * sizeof(png_bytep));
  for(uint32_t i = 0; i < height; i++) {
    data[i] = (uint8_t*)malloc(4 * width*sizeof(png_byte));
  }
};

void Image::freeImageData() {
  if (data == NULL) return;
  for (uint32_t i = 0; i < height; i++) {
    free(data[i]);
  }
  free(data);
};

void Image::showImageData() {
  std::cout << "showImagePixelData:" << std::endl;
  for (uint32_t i = 0; i < height; i++) {
    for (uint32_t j = 0; j < width * 4; j++) {
      std::cout << unsigned(data[i][j]) << " ";
    }
    std::cout << std::endl;
  }
}
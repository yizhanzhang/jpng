#include "image.h"
#include "png.cpp"
#include "jpeg.cpp"

Image::Image() {
  data = NULL;
};

Image::~Image() {
  freeImageData();
};

Image::HandleMap Image::createHanldeMap() {
  Image::HandleMap ret;

  Image::ImageHandle pngHandle = { decodePngBuffer, encodePngBuffer };
  ret.insert({"png", pngHandle});

  Image::ImageHandle jpegHandle = { decodePngBuffer, encodePngBuffer };
  ret.insert({"jpg", jpegHandle});
  ret.insert({"jpeg", jpegHandle});

  return ret;
};

Image::HandleMap Image::handleMap = Image::createHanldeMap();

Result Image::decodeImage(CompressData& inputData) {
  Result result;

  Image::HandleMap::iterator iter = Image::handleMap.find(inputData.type);
  if (iter == Image::handleMap.end()) {
    result.setError("unsupport file format:" + inputData.type);
    return result;
  }

  result = iter->second.decode(*this, inputData);
  return result;
};

Result Image::encodeImage(CompressData& outputData) {
  Result result;

  Image::HandleMap::iterator iter = Image::handleMap.find(outputData.type);
  if (iter == Image::handleMap.end()) {
    result.setError("unsupport file format:" + outputData.type);
    return result;
  }

  result = iter->second.encode(*this, outputData);
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
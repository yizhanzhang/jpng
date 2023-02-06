#ifndef IMAGE_H

#include <cstdio>
#include <iostream>
#include <string>
#include <map>
#include <node_api.h>
#include <assert.h>
#include <sys/stat.h>
#include "png.h"
#include "util.h"

using std::string;

class Image {
  private:
    struct ImageHandle
    {
      Result (*decode)(Image& img, CompressData& inputData);
      Result (*encode)(Image& img, CompressData& outputData);
    };
    typedef std::map<std::string, ImageHandle> HandleMap;
    static HandleMap handleMap;
    static HandleMap createHanldeMap();
  public:
    Image();
    ~Image();
    Result decodeImage(CompressData& inputPath);
    Result encodeImage(CompressData& inputPath);
    uint32_t width;
    uint32_t height;
    uint8_t **data;  // 解码后的位图数据
    void mallocImageData();
    void freeImageData();
    void showImageData();
};

#endif
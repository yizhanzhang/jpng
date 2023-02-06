#ifndef IMAGE_H

#include <cstdio>
#include <iostream>
#include <string>
#include <node_api.h>
#include <assert.h>
#include <sys/stat.h>
#include "png.h"
#include "util.h"

using std::string;

class Image {
  public:
    Image();
    ~Image();
    uint32_t width;
    uint32_t height;
    uint8_t **data;  // 解码后的位图数据
    Result decodeImage(CompressData& inputPath);
    Result encodeImage(CompressData& inputPath);
    void mallocImageData();
    void freeImageData();
    void showImageData();
};

#endif
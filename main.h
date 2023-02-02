#ifndef MAIN_H

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

class Image {
  private:
  public:
    uint32_t width;
    uint32_t height;
    uint8_t **data;  // 解码后的位图数据
    uint8_t *buffer; // 编码后的文件数据
    unsigned long bufferSize;
    Image();
    ~Image();
    void mallocImageData();
    void freeImageData();
    void freeBuffer();
    void showImageData();
};

#endif
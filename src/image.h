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

class Binding {
  public:
    napi_env _env;
    Image* img;
    Binding();
    ~Binding();
    static napi_value DefineNodeClass(napi_env env, napi_value exports);
    static napi_value NodeClassConstructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void* finalize_data, void* finalize_hint);
    static napi_value decode(napi_env env, napi_callback_info info);
    static napi_value encode(napi_env env, napi_callback_info info);
};

#endif
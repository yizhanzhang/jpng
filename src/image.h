#ifndef IMAGE_H

#include <cstdio>
#include <string>
#include <node_api.h>
#include <assert.h>
#include "util.h"

using std::string;

class Image {
  private:
    int id;
  public:
    Image();
    ~Image();
    void convertFormat(string inputPath, string outputPath);
    static napi_value DefineNodeClass(napi_env env, napi_value exports);
    static napi_value NodeClassConstructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void* finalize_data, void* finalize_hint);
    static napi_value getId(napi_env env, napi_callback_info info);
    static napi_value convert(napi_env env, napi_callback_info info);
};

#endif
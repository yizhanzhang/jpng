#include <node_api.h>
#include <string>

class Image {
  private:
    int id;
  public:
    Image();
    ~Image();
    static napi_value DefineNodeClass(napi_env env, napi_value exports);
    static napi_value NodeClassConstructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void* finalize_data, void* finalize_hint);
    static napi_value getId(napi_env env, napi_callback_info info);
};
#include <node_api.h>
#include <assert.h>
#include <string>
#include "image.h"
#include "util.h"

Image::Image() {
  id = rand();
};

Image::~Image() {
};

napi_value Image::NodeDefineClass(napi_env env, napi_value exports) {
  napi_property_descriptor properties[] = {
    { "id", NULL, NULL, Image::getId, NULL, NULL, napi_default, NULL },
  };
  int property_length = sizeof(properties) / sizeof(properties[0]);

  napi_value cons;
  napi_define_class(env, "Image", NAPI_AUTO_LENGTH, Image::NodeClassConstructor, nullptr, property_length, properties, &cons);
  return cons;
};

napi_value Image::NodeClassConstructor(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value js_object;

  size_t argc = 0; 
  napi_value argv;
  status = napi_get_cb_info(env, info, &argc, &argv, &js_object, NULL);
  assert(status == napi_ok);

  Image *img = new Image();
  napi_wrap(env, js_object, reinterpret_cast<void *>(img), Image::Destructor, nullptr, nullptr);
  assert(status == napi_ok);

  return js_object;
};

void Image::Destructor(napi_env env, void* finalize_data, void* finalize_hint) {
  Image *img = reinterpret_cast<Image *>(finalize_data);
  img->~Image();
  delete img;
};

napi_value Image::getId(napi_env env, napi_callback_info info) {
  napi_value js_object;
  napi_get_cb_info(env, info, 0, nullptr, &js_object, nullptr);

  Image *img;
  napi_unwrap(env, js_object, reinterpret_cast<void **>(&img));

  napi_value result;
  napi_create_int32(env, img->id, &result);

  return result;
}

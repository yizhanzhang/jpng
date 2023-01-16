#include <node_api.h>
#include <assert.h>
#include <string>
#include "image.h"
#include "util.h"

using std::string;

napi_value getMoreDetail(napi_env env, napi_callback_info info) {
  napi_value detail_value;
  string detail = "this is yizhan's node-image support!";
  napi_create_string_utf8(env, (char *)detail.c_str(), NAPI_AUTO_LENGTH, &detail_value);
  return detail_value;
};

napi_value init(napi_env env, napi_value exports) {
  // export string
  napi_value name_value;
  napi_create_string_utf8(env, "yizhan-image", NAPI_AUTO_LENGTH,  &name_value);
  napi_set_named_property(env, exports, "name", name_value);

  // export number
  napi_value version_value;
  napi_create_double(env, 0.1, &version_value);
  napi_set_named_property(env, exports, "version", version_value);

  // export method
  napi_value func_value;
  napi_create_function(env, "getMoreDetail", NAPI_AUTO_LENGTH, getMoreDetail, nullptr, &func_value);
  napi_set_named_property(env, exports, "getMoreDetail", func_value);

  // export class
  napi_value ImageCons = Image::DefineNodeClass(env, exports);
  napi_set_named_property(env, exports, "Image", ImageCons);

  return exports;
};

NAPI_MODULE_INIT() {
  return init(env, exports);
}
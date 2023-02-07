#include <node_api.h>
#include <assert.h>
#include <string>
#include "image.h"
#include "util.h"

using std::string;

class Binding {
  public:
    napi_env _env;
    Image* img;
    Binding(napi_env env);
    ~Binding();
    static napi_value DefineNodeClass(napi_env env, napi_value exports);
    static napi_value NodeClassConstructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void* finalize_data, void* finalize_hint);
    static napi_value decode(napi_env env, napi_callback_info info);
    static napi_value encode(napi_env env, napi_callback_info info);
    static napi_value setPixels(napi_env env, napi_callback_info info);
};

Binding::Binding(napi_env env) {
  _env = env;
  img = new Image();
};

Binding::~Binding() {
  delete img;
};

napi_value Binding::DefineNodeClass(napi_env env, napi_value exports) {
  napi_property_descriptor properties[] = {
    { "setPixels", NULL, Binding::setPixels, NULL, NULL, NULL, napi_default, NULL },
    { "decode", NULL, Binding::decode, NULL, NULL, NULL, napi_default, NULL },
    { "encode", NULL, Binding::encode, NULL, NULL, NULL, napi_default, NULL },
  };
  int property_length = sizeof(properties) / sizeof(properties[0]);

  napi_value cons;
  napi_define_class(env, "Image", NAPI_AUTO_LENGTH, Binding::NodeClassConstructor, nullptr, property_length, properties, &cons);
  return cons;
};

napi_value Binding::NodeClassConstructor(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value js_object;

  size_t argc = 0; 
  napi_value argv;
  status = napi_get_cb_info(env, info, &argc, &argv, &js_object, NULL);
  assert(status == napi_ok);

  Binding *obj = new Binding(env);
  napi_wrap(env, js_object, reinterpret_cast<void *>(obj), Binding::Destructor, nullptr, nullptr);
  assert(status == napi_ok);

  return js_object;
};

void Binding::Destructor(napi_env env, void* finalize_data, void* finalize_hint) {
  Binding *obj = reinterpret_cast<Binding *>(finalize_data);
  delete obj;
};

napi_value Binding::decode(napi_env env, napi_callback_info info) {
  napi_value js_object;
  size_t argc = 2;
  napi_value argv[argc];
  napi_get_cb_info(env, info, &argc, argv, &js_object, NULL);
  if (argc < 2) {
    napi_throw_error(env, NULL, "argc is error");
    return NULL;
  }
  if (getValueType(env, argv[0]) != napi_string) {
    napi_throw_type_error(env, NULL, "argv[0] is not string");
    return NULL;
  }
  bool isBuffer;
  napi_is_buffer(env, argv[1], &isBuffer);
  if (!isBuffer) {
    napi_throw_type_error(env, NULL, "argv[0] is not buffer");
    return NULL;
  }
  CompressData inputData = getBufferParam(env, argv[1]);
  inputData.type = getStringParam(env, argv[0]);

  Binding *obj;
  napi_unwrap(env, js_object, reinterpret_cast<void **>(&obj));
  Result result = obj->img->decodeImage(inputData);
  if (result.flag == -1) {
    napi_throw_error(obj->_env, NULL, result.err.c_str());
  }

  return NULL;
};

napi_value Binding::encode(napi_env env, napi_callback_info info) {
  napi_value js_object;
  size_t argc = 2;
  napi_value argv[argc];
  napi_get_cb_info(env, info, &argc, argv, &js_object, NULL);
  if (argc < 1) {
    napi_throw_error(env, NULL, "argc is error");
    return NULL;
  }
  if (getValueType(env, argv[0]) != napi_string) {
    napi_throw_type_error(env, NULL, "argv[0] is not string");
    return NULL;
  }
  CompressData outputData;
  outputData.type = getStringParam(env, argv[0]);

  Binding *obj;
  napi_unwrap(env, js_object, reinterpret_cast<void **>(&obj));
  Result result = obj->img->encodeImage(outputData);
  if (result.flag == -1) {
    napi_throw_error(obj->_env, NULL, result.err.c_str());
  }

  napi_value buffer;
  napi_create_buffer_copy(env, outputData.length, outputData.buffer, nullptr, &buffer);

  return buffer;
};

napi_value Binding::setPixels(napi_env env, napi_callback_info info) {
  napi_value js_object;
  size_t argc = 3;
  napi_value argv[argc];
  napi_get_cb_info(env, info, &argc, argv, &js_object, NULL);
  if (argc < 3) {
    napi_throw_error(env, NULL, "argc is error");
    return NULL;
  }
  if (getValueType(env, argv[0]) != napi_number) {
    napi_throw_type_error(env, NULL, "argv[0] is not number");
    return NULL;
  }
  if (getValueType(env, argv[1]) != napi_number) {
    napi_throw_type_error(env, NULL, "argv[1] is not number");
    return NULL;
  }
  uint32_t width;
  napi_get_value_uint32(env, argv[0], &width);
  uint32_t height;
  napi_get_value_uint32(env, argv[1], &height);
  uint32_t arrayLength = 0;
  napi_get_array_length(env, argv[2], &arrayLength);
  if (width * height * 4 != arrayLength) {
    napi_throw_type_error(env, NULL, "data size is error");
    return NULL;
  }
  uint8_t **data= (uint8_t **)malloc(height * sizeof(uint8_t *));
  for (uint32_t i = 0; i < height; i++) {
    data[i] = (uint8_t *)malloc(width * 4 * sizeof(uint8_t));
  }
  for (uint32_t i = 0; i < height; i++) {
    for (uint32_t j = 0; j < width * 4; j++) {
      uint32_t index = i * width * 4 + j;
      napi_value e;
      napi_get_element(env, argv[2], index, &e);
      uint32_t v = 0;
      napi_get_value_uint32(env, e, &v);
      data[i][j] = v;
    }
  }

  Binding *obj;
  napi_unwrap(env, js_object, reinterpret_cast<void **>(&obj));
  Result result = obj->img->setPixels(width, height, data);
  if (result.flag == -1) {
    napi_throw_error(obj->_env, NULL, result.err.c_str());
  }

  return NULL;
}

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
  napi_value BindingImageCons = Binding::DefineNodeClass(env, exports);
  napi_set_named_property(env, exports, "Image", BindingImageCons);

  return exports;
};

NAPI_MODULE_INIT() {
  return init(env, exports);
}
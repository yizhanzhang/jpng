#include "image.h"
#include "png.cpp"
#include "jpeg.cpp"

Binding::Binding() {
  img = new Image();
};

Binding::~Binding() {
  delete img;
};

napi_value Binding::DefineNodeClass(napi_env env, napi_value exports) {
  napi_property_descriptor properties[] = {
    { "decode", NULL, Binding::decode, NULL, NULL, NULL, napi_default, NULL },
    { "encode", NULL, Binding::encode, NULL, NULL, NULL, napi_default, NULL }
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

  Binding *obj = new Binding();
  napi_wrap(env, js_object, reinterpret_cast<void *>(obj), Binding::Destructor, nullptr, nullptr);
  assert(status == napi_ok);
  obj->_env = env;

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


Image::Image() {
  data = NULL;
};

Image::~Image() {
  freeImageData();
};

Result Image::decodeImage(CompressData& inputData) {
  Result result;

  Result (*decodeFunc)(Image &img, CompressData& inputData);

  if (inputData.type == "png") {
    decodeFunc = decodePngBuffer;
  } else if(inputData.type == "jpg"|| inputData.type == "jpeg") {
    decodeFunc = decodeJpegBuffer;
  } else {
    result.setError("unsupport file format:" + inputData.type);
    return result;
  }

  result = decodeFunc(*this, inputData);
  return result;
};

Result Image::encodeImage(CompressData& outputData) {
  Result result;
  Result (*encodeFunc)(Image &img, CompressData& outputData);
  if (outputData.type == "png") {
    encodeFunc = encodePngBuffer;
  } else if(outputData.type == "jpg"|| outputData.type == "jpeg") {
    encodeFunc = encodeJpegBuffer;
  } else {
    result.setError("unsupport file format:" + outputData.type);
    return result;
  }

  result = encodeFunc(*this, outputData);
  if(result.flag == -1) {
    return result;
  }

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
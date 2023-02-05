#include "image.h"
#include "png.cpp"

Binding::Binding() {
  img = new Image();
};

Binding::~Binding() {
  delete img;
};

napi_value Binding::DefineNodeClass(napi_env env, napi_value exports) {
  napi_property_descriptor properties[] = {
    { "convert", NULL, Binding::convert, NULL, NULL, NULL, napi_default, NULL }
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

napi_value Binding::convert(napi_env env, napi_callback_info info) {
  napi_value js_object;
  size_t argc = 2;
  napi_value argv[argc];
  napi_get_cb_info(env, info, &argc, argv, &js_object, NULL);
  if (argc < 2) {
    napi_throw_error(env, NULL, "argc is error");
    return NULL;;
  }
  if (getValueType(env, argv[0]) != napi_string) {
    napi_throw_type_error(env, NULL, "argv[0] is not string");
    return NULL;;
  }
  if (getValueType(env, argv[1]) != napi_string) {
    napi_throw_type_error(env, NULL, "argv[1] is not string");
    return NULL;;
  }
  string inputPath = getStringParam(env, argv[0]);
  string outputPath = getStringParam(env, argv[1]);

  Binding *obj;
  napi_unwrap(env, js_object, reinterpret_cast<void **>(&obj));
  Result result = obj->img->convertFormat(inputPath, outputPath);
  if (result.flag == -1) {
    napi_throw_error(obj->_env, NULL, result.err.c_str());
  }

  return NULL;
};


Image::Image() {
  data = NULL;
  buffer = NULL;
};

Image::~Image() {
  freeBuffer();
  freeImageData();
};

Result decodeImage(Image* img, string filepath) {
  Result result;
  struct stat statbuf;
  if (stat(filepath.c_str(), &statbuf) != 0) {
    result.setError("find file error:" + filepath);
    return result;
  };
  size_t filesize = statbuf.st_size;

  FILE *fp = fopen((char *)filepath.c_str(), "rb");
  if (!fp) {
    result.setError("open file error:" + filepath);
    return result;
  }
  uint8_t buffer[filesize];
  fread(buffer, filesize, 1, fp);
  fclose(fp);
  
  if (endsWith(filepath, ".png")) {
    decodePngBuffer(*img, buffer, filesize);
  } else if(endsWith(filepath, ".jpg") || endsWith(filepath, ".jpeg")) {
    // decodeJpegBuffer(img, buffer, filesize);
  } else {
    result.setError("unsupport file format" + filepath);
    return result;
  }
  return result;
};

Result encodeImage(Image* img, string filepath) {
  Result result;
  Result (*encodeFunc)(Image &img);
  if (endsWith(filepath, ".png")) {
    encodeFunc = encodePngBuffer;
  // } else if(endsWith(filepath, ".jpg") || endsWith(filepath, ".jpeg")) {
    // encodeJpegBuffer(img);
    // encodeFunc = encodeJpegBuffer;
  } else {
    result.setError("unsupport file format" + filepath);
    return result;
  }

  result = encodeFunc(*img);
  if(result.flag == -1) {
    return result;
  }

  FILE *fp = fopen((char *)filepath.c_str(), "wb");
  if (!fp) {
    result.setError("open file error:" + filepath);
    return result;
  }
  fwrite((*img).buffer, (*img).bufferSize, 1, fp);
  fclose(fp);

  return result;
};

Result Image::convertFormat(string inputPath, string outputPath) {
  Result result;
  result = decodeImage(this, inputPath);
  if (result.flag == -1) return result;
  result = encodeImage(this, outputPath);
  if (result.flag == -1) return result;
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

void Image::freeBuffer() {
  if (data == NULL) return;
  free(buffer);
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
#include "image.h"
// #include "png.cpp"

struct ImageIOHost {
  int position;
  uint8_t *data;
};

void read_png_data_fn(png_structp png_ptr, png_bytep data, size_t length) {
  ImageIOHost *imgHost = (ImageIOHost *)png_get_io_ptr(png_ptr);
  memcpy(data, &(imgHost->data[imgHost->position]), length);
  imgHost->position += length;
}
void decodePngBuffer(Image& img, uint8_t *buffer, size_t size) {
  if(png_sig_cmp(buffer, 0, 4)) {
    return void(0);
  }

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  png_infop info_ptr = png_create_info_struct(png_ptr);

  ImageIOHost imgHost;
  imgHost.position = 0;
  imgHost.data = buffer;
  png_set_read_fn(png_ptr, (void *)&imgHost, read_png_data_fn);
  png_read_info(png_ptr, info_ptr);
  int bit_depth, color_type;
  png_get_IHDR(png_ptr, info_ptr, &img.width, &img.height, &bit_depth, &color_type, NULL, NULL, NULL);
  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    png_set_expand(png_ptr);
  } else if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
    png_set_gray_to_rgb(png_ptr);
  }
  if (color_type != PNG_COLOR_TYPE_RGB_ALPHA && color_type != PNG_COLOR_TYPE_GRAY_ALPHA) {
    png_set_add_alpha(png_ptr, 255, PNG_FILTER_NONE);
  }
  if (bit_depth < 8) {
    png_set_packing(png_ptr);
  } else if (bit_depth == 16) {
    png_set_strip_16(png_ptr);
  }
  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
    png_set_tRNS_to_alpha(png_ptr);
  }
  png_read_update_info(png_ptr, info_ptr);
  bit_depth = png_get_bit_depth(png_ptr, info_ptr);
  color_type = png_get_color_type(png_ptr, info_ptr);

  img.mallocImageData();
  png_read_image(png_ptr, img.data);
  png_read_end(png_ptr, info_ptr);
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
}

void write_date_fn(png_structp png_ptr, png_bytep data, size_t length) {
  ImageIOHost *imgHost = (ImageIOHost *)png_get_io_ptr(png_ptr);
  size_t wholeLength = imgHost->position + length;
  imgHost->data = (uint8_t *)realloc(imgHost->data, wholeLength);
  memcpy(&imgHost->data[imgHost->position], data, length);
  imgHost->position += length;
}
void encodePngBuffer(Image& img) {
  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  png_infop info_ptr = png_create_info_struct(png_ptr);

  ImageIOHost imgHost;
  imgHost.position = 0;
  imgHost.data = (uint8_t *)malloc(0);
  png_set_write_fn(png_ptr, (void *)&imgHost, write_date_fn, NULL);
  png_set_IHDR(png_ptr, info_ptr, img.width, img.height, 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
  png_write_info(png_ptr, info_ptr);
  png_write_image(png_ptr, img.data);
  png_write_end(png_ptr, info_ptr);
  img.buffer = imgHost.data;
  img.bufferSize = imgHost.position;

  png_destroy_write_struct(&png_ptr, &info_ptr);
};

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
    result.flag = -1;
    result.err = "find file error:" + filepath;
    return result;
  };
  size_t filesize = statbuf.st_size;

  FILE *fp = fopen((char *)filepath.c_str(), "rb");
  if (!fp) {
    result.flag = -1;
    result.err = "open file error:" + filepath;
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
    result.flag = -1;
    result.err = "unsupport file format" + filepath;
    return result;
  }
  return result;
};

Result encodeImage(Image* img, string filepath) {
  Result result;
  if (endsWith(filepath, ".png")) {
    encodePngBuffer(*img);
  } else if(endsWith(filepath, ".jpg") || endsWith(filepath, ".jpeg")) {
    // encodeJpegBuffer(img);
  } else {
    result.flag = -1;
    result.err = "unsupport file format" + filepath;
    return result;
  }

  FILE *fp = fopen((char *)filepath.c_str(), "wb");
  if (!fp) {
    result.flag = -1;
    result.err = "open file error:" + filepath;
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
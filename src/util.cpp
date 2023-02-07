#include "util.h"

CompressData::CompressData() {
  buffer = NULL;
  length = 0;
};

void Result::setError(std::string msg) {
  flag = -1;
  err = msg;
};

napi_valuetype getValueType(napi_env env, napi_value value) {
  napi_valuetype valuetype;
  napi_typeof(env, value, &valuetype);
  return valuetype;
};

std::string getStringParam(napi_env env, napi_value value) {
  size_t bufsize = 0;
  napi_get_value_string_utf8(env, value, nullptr, 0, &bufsize);
  char buf[bufsize + 1];
  napi_get_value_string_utf8(env, value, buf, bufsize+1, &bufsize);
  std::string str = buf;
  return str;
}

CompressData getBufferParam(napi_env env, napi_value value) {
  CompressData data;
  napi_get_buffer_info(env, value, (void **)&data.buffer, &data.length);
  return data;
}

bool endsWith(std::string const &str, std::string const &suffix) {
  if (str.length() < suffix.length()) {
      return false;
  }
  return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

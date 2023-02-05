#include "util.h"

struct Result
{
  bool flag;
  std::string err;
};

napi_valuetype getValueType(napi_env env, napi_value value) {
  napi_valuetype valuetype;
  napi_typeof(env, value, &valuetype);
  return valuetype;
};

std::string getStringParam(napi_env env, napi_value value) {
  size_t bufsize = 1024;
  char buf[bufsize];
  size_t result;
  napi_get_value_string_utf8(env, value, buf, bufsize, &result);
  if (result >= bufsize) {
    napi_throw_error(env, NULL, "getStringParam oversize");
    return NULL;
  };
  std::string str = buf;
  return str;
}

bool endsWith(std::string const &str, std::string const &suffix) {
  if (str.length() < suffix.length()) {
      return false;
  }
  return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

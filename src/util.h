#ifndef UTIL_H
#define UTIL_H

#include <node_api.h>
#include <string>

struct Result
{
  int flag;
  std::string err;
  void setError(std::string msg);
};

#define GET_VALUE_FROM_ARGS(convert, argv, value_ref) do { \
  napi_status status; \
  napi_valuetype valuetype; \
  status = napi_typeof(env, argv, &valuetype); \
  assert(status == napi_ok); \
  if (valuetype != napi_undefined) { \
    status = convert(env, argv, value_ref); \
    assert(status == napi_ok); \
  } \
} while(0); \

#define GET_STRING_FROM_ARGS(convert, argv, value_ref) do { \
  napi_status status; \
  napi_valuetype valuetype; \
  status = napi_typeof(env, argv, &valuetype); \
  assert(status == napi_ok); \
  if (valuetype != napi_undefined) { \
    char buf[9999]; \
    size_t buf_size = 9999; \
    size_t valid_length = 0; \
    status = convert(env, argv, buf, buf_size, &valid_length); \
    assert(status == napi_ok); \
    *value_ref = buf; \
  } \
} while(0); \

napi_valuetype getValueType(napi_env env, napi_value value);

std::string getStringParam(napi_env env, napi_value value);

bool endsWith(std::string const &str, std::string const &suffix);

#endif
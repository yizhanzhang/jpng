#ifndef UTIL_H

#include <node_api.h>

#define GET_VALUE_FROM_ARGS(convert, argv, value_ref) do { \
  napi_valuetype valuetype; \
  status = napi_typeof(env, argv, &valuetype); \
  assert(status == napi_ok); \
  if (valuetype != napi_undefined) { \
    status = convert(env, argv, value_ref); \
    assert(status == napi_ok); \
  } \
} while(0); \

#define GET_STRING_FROM_ARGS(convert, argv, value_ref) do { \
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

#endif
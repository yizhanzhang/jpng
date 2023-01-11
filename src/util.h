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

#endif
#ifndef UTIL_H
#define UTIL_H

#include <node_api.h>
#include <string>

struct CompressData {
  std::string type;
  uint8_t *buffer;
  size_t length;
  CompressData();
};

struct Result
{
  int flag;
  std::string err;
  void setError(std::string msg);
};

napi_valuetype getValueType(napi_env env, napi_value value);
std::string getStringParam(napi_env env, napi_value value);
CompressData getBufferParam (napi_env env, napi_value value);

bool endsWith(std::string const &str, std::string const &suffix);

#endif
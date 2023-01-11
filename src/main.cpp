#include <node_api.h>
#include <assert.h>
#include <string>
#include "util.h"

using std::string;

class Student {
  private:
  public:
    int id;
    int age;
    string name;
    napi_env env_;
    napi_ref wrapper_;
    Student(int id, int age, string name);
    ~Student();
    static void Destructor(napi_env env, void* finalize_data, void* finalize_hint);
    static napi_value New(napi_env env, napi_callback_info info);
    static napi_value getId(napi_env env, napi_callback_info info);
    static napi_value getAge(napi_env env, napi_callback_info info);
    static napi_value setAge(napi_env env, napi_callback_info info);
    static napi_value getName(napi_env env, napi_callback_info info);
};

Student::Student(int inputId, int inputAge, string inputName) {
  id = inputId;
  age = inputAge;
  name = inputName;
};

Student::~Student() {};

void Student::Destructor(napi_env env, void* finalize_data, void* finalize_hint) {
  Student *stu = reinterpret_cast<Student *>(finalize_data);
  stu->~Student();
  delete stu;
};

napi_value Student::New(napi_env env, napi_callback_info info) {
  size_t argc = 3;
  napi_status status;
  napi_value args[3];
  napi_value jsthis;

  status = napi_get_cb_info(env, info, &argc, args, &jsthis, NULL);
  assert(status == napi_ok);

  int32_t id = 0;
  int32_t age = 0;
  string name = "";
  GET_VALUE_FROM_ARGS(napi_get_value_int32, args[0], &id);
  GET_VALUE_FROM_ARGS(napi_get_value_int32, args[1], &age);
  GET_STRING_FROM_ARGS(napi_get_value_string_utf8, args[2], &name);
  Student *stu = new Student((int)id, (int)age, name);

  stu->env_ = env;
  status = napi_wrap(env, jsthis, reinterpret_cast<void *>(stu), Student::Destructor, nullptr, &stu->wrapper_);
  assert(status == napi_ok);

  return jsthis;
}

napi_value Student::getId(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value jsthis;

  status = napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr);
  assert(status == napi_ok);

  Student *stu;
  status = napi_unwrap(env, jsthis, reinterpret_cast<void **>(&stu));
  assert(status == napi_ok);

  napi_value value;
  status = napi_create_int32(env, stu->id, &value);
  assert(status == napi_ok);

  return value;
};

napi_value Student::getAge(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value jsthis;

  status = napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr);
  assert(status == napi_ok);

  Student *stu;
  status = napi_unwrap(env, jsthis, reinterpret_cast<void **>(&stu));
  assert(status == napi_ok);

  napi_value result;
  status = napi_create_int32(env, stu->age, &result);
  assert(status == napi_ok);

  return result;
};

napi_value Student::setAge(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value jsthis;
  size_t argc = 1;
  napi_value argv;

  status = napi_get_cb_info(env, info, &argc, &argv, &jsthis, nullptr);
  assert(status == napi_ok);

  Student *stu;
  status = napi_unwrap(env, jsthis, reinterpret_cast<void **>(&stu));
  assert(status == napi_ok);

  GET_VALUE_FROM_ARGS(napi_get_value_int32, argv, &stu->age);

  return argv;
};

napi_value Student::getName(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value jsthis;

  status = napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr);
  assert(status == napi_ok);

  Student *stu;
  status = napi_unwrap(env, jsthis, reinterpret_cast<void **>(&stu));
  assert(status == napi_ok);

  napi_value result;
  string name = "yizhanzhang";
  napi_create_string_utf8(env, (char *)name.c_str(), name.length(), &result);
  assert(status == napi_ok);

  return result;
}

napi_value init(napi_env env, napi_value exports) {
  napi_value result;
  napi_create_uint32(env, 0, &result);
  napi_set_named_property(env, exports, "id", result);

  napi_value cons;
  napi_property_descriptor properties[] = {
    { "id", NULL, NULL, Student::getId, NULL, NULL, napi_default, NULL },
    { "age", NULL, NULL, Student::getAge, Student::setAge, NULL, napi_default, NULL },
    { "name", NULL, NULL, Student::getName, NULL, NULL, napi_default, NULL },
  };
  napi_define_class(env, "Student", NAPI_AUTO_LENGTH, Student::New, NULL, sizeof(properties) / sizeof(properties[0]), properties, &cons);
  napi_set_named_property(env, exports, "Student", cons);

  return exports;
};

NAPI_MODULE_INIT() {
  return init(env, exports);
}
#include "view.h"
#include "napi-common.h"
#include <vector>

napi_ref View::constructor;

napi_env View::event_env_ = nullptr;

View::View()
    : env_(nullptr), wrapper_(nullptr), window(SW_TITLEBAR | SW_RESIZEABLE | SW_CONTROLS | SW_MAIN | SW_GLASSY | SW_ENABLE_DEBUG) {}

View::~View() { napi_delete_reference(env_, wrapper_); }

void View::Destructor(
  napi_env env, void* nativeObject, void* /*finalize_hint*/) {
  View* obj = static_cast<View*>(nativeObject);
  //delete obj;
  obj->release();
}

void View::Init(napi_env env, napi_value exports) {
  napi_property_descriptor properties[] = {
    //{ "value", nullptr, nullptr, GetValue, SetValue, 0, napi_default, 0 },
    DECLARE_NAPI_PROPERTY("loadFile", loadFile),
    DECLARE_NAPI_PROPERTY("loadHtml", loadHtml),
    DECLARE_NAPI_PROPERTY("call", call),
    DECLARE_NAPI_PROPERTY("expand", expand),
    DECLARE_NAPI_PROPERTY("collapse",collapse),
  };

  napi_value cons;
  NAPI_CALL_RETURN_VOID(env, napi_define_class(
      env, "View", -1, New, nullptr, _countof(properties) , properties, &cons));

  NAPI_CALL_RETURN_VOID(env, napi_create_reference(env, cons, 1, &constructor));

  NAPI_CALL_RETURN_VOID(env,
      napi_set_named_property(env, exports, "View", cons));
}

napi_value View::New(napi_env env, napi_callback_info info) {
  napi_value new_target;
  NAPI_CALL(env, napi_get_new_target(env, info, &new_target));
  bool is_constructor = (new_target != nullptr);

  size_t argc = 1;
  napi_value args[1];
  napi_value _this;
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &_this, nullptr));

  if (is_constructor) {
    // Invoked as constructor: `new View(...)`
    //double value = 0;

    napi_valuetype valuetype;
    NAPI_CALL(env, napi_typeof(env, args[0], &valuetype));

    //if (valuetype != napi_undefined) {
    //  NAPI_CALL(env, napi_get_value_double(env, args[0], &value));
    //}

    View* obj = new View();

    obj->env_ = env;
    NAPI_CALL(env, napi_wrap(env,
                             _this,
                             obj,
                             View::Destructor,
                             nullptr,  // finalize_hint
                             &obj->wrapper_));

    return _this;
  }

  // Invoked as plain function `View(...)`, turn into construct call.
  argc = 1;
  napi_value argv[1] = {args[0]};

  napi_value cons;
  NAPI_CALL(env, napi_get_reference_value(env, constructor, &cons));

  napi_value instance;
  NAPI_CALL(env, napi_new_instance(env, cons, argc, argv, &instance));

  return instance;
}

/*napi_value View::GetValue(napi_env env, napi_callback_info info) {
  napi_value _this;
  NAPI_CALL(env,
      napi_get_cb_info(env, info, nullptr, nullptr, &_this, nullptr));

  View* obj;
  NAPI_CALL(env, napi_unwrap(env, _this, reinterpret_cast<void**>(&obj)));

  napi_value num;
  NAPI_CALL(env, napi_create_double(env, obj->value_, &num));

  return num;
}

napi_value View::SetValue(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value args[1];
  napi_value _this;
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &_this, nullptr));

  View* obj;
  NAPI_CALL(env, napi_unwrap(env, _this, reinterpret_cast<void**>(&obj)));

  NAPI_CALL(env, napi_get_value_double(env, args[0], &obj->value_));

  return nullptr;
}*/

napi_value View::loadFile(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value args[1];
  napi_value _this;
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &_this, nullptr));

  View* obj;
  NAPI_CALL(env, napi_unwrap(env, _this, reinterpret_cast<void**>(&obj)));

  bool r = false;

  if (argc >= 1) 
  {
    char16_t  buf[2048] = {0};
    size_t length = 0;
    NAPI_CALL(env,napi_get_value_string_utf16(env, args[0], buf, sizeof(buf), &length));
    // very basic path to file URL conversion
    std::replace(&buf[0], &buf[length], '\\', '/');
    sciter::string url = WSTR("file://");
    url += LPCWSTR(buf);
    r = obj->load_file(url.c_str());
  }

  napi_value res;
  NAPI_CALL(env, napi_get_boolean(env, r, &res));

  return res;
}

napi_value View::loadHtml(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value args[1];
  napi_value _this;
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &_this, nullptr));

  View* obj;
  NAPI_CALL(env, napi_unwrap(env, _this, reinterpret_cast<void**>(&obj)));

  bool r = false;

  std::vector<BYTE> buf;
  std::wstring      url;

  if (argc >= 1)
  {
    size_t length = 0;
    NAPI_CALL(env, napi_get_value_string_utf8(env, args[0], NULL, 0, &length));
    buf.resize(length+1);
    NAPI_CALL(env, napi_get_value_string_utf8(env, args[0], (char*)buf.data(), buf.size(), &length));
  }
  if (argc >= 2)
  {
    char16_t  buf[2048] = { 0 };
    size_t length = 0;
    NAPI_CALL(env, napi_get_value_string_utf16(env, args[1], buf, sizeof(buf), &length));
    url = (const wchar_t*)buf;
  }

  r = obj->load_html(buf.data(),(UINT)buf.size(),url.c_str());

  napi_value res;
  NAPI_CALL(env, napi_get_boolean(env, r, &res));

  return res;
}

napi_value View::expand(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value args[1];
  napi_value _this;
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &_this, nullptr));

  bool maximize = false;
  if (argc >= 1) {
    NAPI_CALL(env, napi_get_value_bool(env, args[0], &maximize));
  }

  View* obj;
  NAPI_CALL(env, napi_unwrap(env, _this, reinterpret_cast<void**>(&obj)));

  obj->window::expand(maximize);

  napi_value ret;
  NAPI_CALL(env, napi_get_undefined(env, &ret));
  return ret;
}

napi_value View::collapse(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value args[1];
  napi_value _this;
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &_this, nullptr));

  View* obj;
  NAPI_CALL(env, napi_unwrap(env, _this, reinterpret_cast<void**>(&obj)));

  obj->window::collapse();

  napi_value ret;
  NAPI_CALL(env, napi_get_undefined(env, &ret));
  return ret;
}

napi_value View::call(napi_env env, napi_callback_info info) 
{
  size_t     argc = 16;
  napi_value args[16];
  napi_value _this;
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &_this, nullptr));

  View* obj;
  NAPI_CALL(env, napi_unwrap(env, _this, reinterpret_cast<void**>(&obj)));

  NAPI_ASSERT(env, argc >= 1, "too few parameters");

  sciter::astring func_path = cvt_string8(env, args[0]);
  sciter::value r;

  if (argc > 1) {
    std::vector<sciter::value> fargs = cvt(env, aux::slice<napi_value>(&args[1], argc - 1));
    r = obj->call_function(func_path.c_str(), UINT(fargs.size()), fargs.data());
  }
  else {
    r = obj->call_function(func_path.c_str());
  }
  return cvt(env, r);
}

bool  View::handle_scripting_call(HELEMENT he, SCRIPTING_METHOD_PARAMS& params) 
{
  assert(env_);
  if (!env_)
    return false;

  napi_value _this = nullptr; 
  napi_get_reference_value(env_, wrapper_,&_this);
    
  napi_value method = nullptr;
  napi_status stat = napi_get_named_property(env_,_this, params.name, &method);
  if (stat != napi_ok)
    return false;
  napi_valuetype vt;
  napi_typeof(env_, method, &vt);
  if (vt != napi_function)
    return false;

  napi_value res;
  std::vector<napi_value> jargs = cvt(env_, aux::slice<sciter::value>(params.argv, params.argc));
  napi_call_function(env_, _this, method, jargs.size(), jargs.data(), &res);

  params.result = cvt(env_, res);
  return true;
}

#ifndef _SCITER_NODE_VIEW_H_
#define _SCITER_NODE_VIEW_H_

#include "node_api.h"
#include "sciter-x.h"
#include "sciter-x-window.hpp"

napi_value      cvt(napi_env env, sciter::value val);
sciter::value   cvt(napi_env env, napi_value val);
sciter::astring cvt_string8(napi_env env, napi_value val);
sciter::string  cvt_string16(napi_env env, napi_value val);

std::vector<sciter::value> cvt(napi_env env, aux::slice<napi_value> args);
std::vector<napi_value>    cvt(napi_env env, aux::slice<sciter::value> args);

class View: public sciter::window {
 public:
  static void Init(napi_env env, napi_value exports);
  static void Destructor(napi_env env, void* nativeObject, void* finalize_hint);

  static napi_env event_env_;

 private:
  explicit View();
  ~View();

  virtual bool handle_scripting_call(HELEMENT he, SCRIPTING_METHOD_PARAMS& params) override;

  static napi_value New(napi_env env, napi_callback_info info);
  //static napi_value GetValue(napi_env env, napi_callback_info info);
  //static napi_value SetValue(napi_env env, napi_callback_info info);
  static napi_value loadHtml(napi_env env, napi_callback_info info);
  static napi_value loadFile(napi_env env, napi_callback_info info);
  static napi_value expand(napi_env env, napi_callback_info info);
  static napi_value collapse(napi_env env, napi_callback_info info);
  static napi_value call(napi_env env, napi_callback_info info);
  static napi_ref   constructor;
  //double value_;
  napi_env env_;
  napi_ref wrapper_;

};

#endif  // _SCITER_NODE_VIEW_H_

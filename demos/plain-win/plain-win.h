#pragma once

#include "resource.h"
#include "sciter-x.h"
#include "sciter-x-host-callback.h"
#include "sciter-x-request.hpp"

extern HINSTANCE ghInstance;

class window: public sciter::host<window>
{
  HWND _hwnd;

  static LRESULT CALLBACK	wnd_proc(HWND, UINT, WPARAM, LPARAM);
  static window* ptr(HWND hwnd);
  static bool init_class();
public:
  // notification_handler traits:
  HWND      get_hwnd() const { return _hwnd; }
  HINSTANCE get_resource_instance() const{ return ghInstance; }

  /* sciter::request / LOAD_MYSELF testing
  LRESULT on_load_data(LPSCN_LOAD_DATA pnmld) {
    aux::wchars wu = aux::chars_of(pnmld->uri);
    if(wu == const_wchars("res:go.png"))
    {
      LPCBYTE pb = 0; UINT cb = 0;
      load_resource_data(wu.start+4, pb, cb);
      sciter::request rq( pnmld->requestId );
      rq.succeeded(200,pb,cb);
      return LOAD_MYSELF;
    }
    return sciter::host<window>::on_load_data(pnmld);
  }*/ 

  window();
  bool init(); // instance
  bool is_valid() const { return _hwnd != 0; }
};

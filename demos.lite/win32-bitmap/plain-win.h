#pragma once

#include "sciter-x.h"
#include "sciter-x-host-callback.h"
#include "sciter-x-request.hpp"
#include "sciter-x-dom.hpp"
#include "sciter-x-behavior.h"

extern HINSTANCE ghInstance;

class window :
  public sciter::host<window>,
  public sciter::event_handler
{
  HWND hwnd;
  bool loaded;
  bool mouseEnter;

  HDC memDc;
  HBITMAP memBm;
  SIZE memSize;

public:
  // notification_handler traits:
  HWINDOW   get_hwnd() const { return (HWINDOW)this; }
  HINSTANCE get_resource_instance() const { return ghInstance; }
  LRESULT handle_notification(LPSCITER_CALLBACK_NOTIFICATION pnm);

  virtual LRESULT on_load_data(LPSCN_LOAD_DATA pnmld) override;
  virtual bool handle_scripting_call(HELEMENT he, SCRIPTING_METHOD_PARAMS& params) override;

  /* sciter::request / LOAD_MYSELF testing */
  /*LRESULT on_load_data(LPSCN_LOAD_DATA pnmld) {
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

  bool is_valid() const { return hwnd != 0; }

protected:
  static LRESULT CALLBACK	wnd_proc(HWND, UINT, WPARAM, LPARAM);
  static window* ptr(HWND hwnd);

  static bool init_class();

  bool init(); // instance

  void invalidate_window();

  void paint();

  static unsigned translate_key(unsigned vk);
};

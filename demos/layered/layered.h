#pragma once

#include "resource.h"

#include "sciter-x.h"
#include "sciter-x-host-callback.h"

extern HINSTANCE ghInstance;

class window: public sciter::host<window>
{
  HWND _hwnd;

  static LRESULT CALLBACK	wnd_proc(HWND, UINT, WPARAM, LPARAM);
  static window* ptr(HWND hwnd);
  static bool init_class();
public:
  // notification_handler traits:
  HWND      get_hwnd() { return _hwnd; }
  HINSTANCE get_resource_instance() { return ghInstance; }

  window();
  bool init(); // instance
  bool is_valid() const { return _hwnd != 0; }
};

#pragma once

#include "resource.h"

#include <functional>
#include "sciter-x.h"
#include "sciter-x-host-callback.h"
#include "sciter-x-threads.h"

extern HINSTANCE ghInstance;

class window: public sciter::host<window>,
              public sciter::event_handler // in order BEGIN_FUNCTION_MAP/END_FUNCTION_MAP to work
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

  // declaration of methods to be called from script
  BEGIN_FUNCTION_MAP 
    FUNCTION_3("execTask", exec_task); // function view.execTask(taskId, progressCb, doneCb); - 3 args
  END_FUNCTION_MAP

  sciter::value exec_task(sciter::value taskId, sciter::value progressCb, sciter::value doneCb);

};




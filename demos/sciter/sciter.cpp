// plain-win.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "sciter.h"
#include "sciter-x-dom.hpp"
#include "sciter-x-request.hpp"
#include <process.h>

//#pragma comment(lib, "dwmapi.lib")

extern HINSTANCE ghInstance;

namespace sciter
{
  
  #define WINDOW_CLASS_NAME L"sciter-frame"			// the main frame class name
    
  //
  //  FUNCTION: frame::init()
  //
  //  PURPOSE: Registers the frame class.
  //
  bool frame::init_class()
  {
    
    static ATOM cls = 0;
    if( cls ) 
      return true;

	  WNDCLASSEX wcex;

	  wcex.cbSize = sizeof(WNDCLASSEX);

	  wcex.style			    = CS_HREDRAW | CS_VREDRAW;
	  wcex.lpfnWndProc	  = wnd_proc;
	  wcex.cbClsExtra		  = 0;
	  wcex.cbWndExtra		  = 0;
	  wcex.hInstance		  = ghInstance;
	  wcex.hIcon			    = LoadIcon(ghInstance, MAKEINTRESOURCE(IDI_SCITER));
	  wcex.hCursor		    = LoadCursor(NULL, IDC_ARROW);
	  wcex.hbrBackground	= NULL;//(HBRUSH)(COLOR_WINDOW+1);
	  wcex.lpszMenuName	  = 0;//MAKEINTRESOURCE(IDC_PLAINWIN);
	  wcex.lpszClassName	= WINDOW_CLASS_NAME;
	  wcex.hIconSm		    = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	  cls = RegisterClassEx(&wcex);
    return cls != 0;
  }

  frame* frame::_first = 0;
  sync::mutex frame::_guard;

  frame::frame():_hwnd(0),_next(0),_prev(0)
  {
    sync::critical_section _(_guard);
    init_class();
    if(_first) 
      _first->_prev = this;
    _next = _first;
    _first = this;
    add_ref();
  }

  bool frame::setup(const wchar_t* url, HWND parent)
  {
    _hwnd = CreateWindowEx(0,WINDOW_CLASS_NAME, L"sciter-frame", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, parent, NULL, ghInstance, this);
    
    if (!_hwnd)
      return false;
     
    SetWindowLongPtr(_hwnd, GWLP_USERDATA, LONG_PTR(this));
    setup_callback(); // to receive SC_LOAD_DATA, SC_DATA_LOADED, etc. notification
    attach_dom_event_handler(_hwnd,this); // to receive DOM events

//#ifdef _DEBUG    
//    SciterSetMediaType(_hwnd,L"on-debug");
//#endif    

    init_window();

    load_file(url);

    dom::element root_el = root();

    //json::string s = root_el.combine_url(json::string());

    assert(root_el.is_valid());
    if(root_el)
    {
      init_instance(root_el);
      if(dom::element title_el = root_el.find_first(":root>head>title"))
        set_title(title_el.text());
    }     
    
    ShowWindow(_hwnd, SW_SHOW);
    UpdateWindow(_hwnd);
    
    return true;
  }

  void frame::forget(bool and_release)
  {
    {
      sync::critical_section _(_guard);
      if( this == _first )
        _first = _next;
      if(_next) _next->_prev = _prev;
      if(_prev) _prev->_next = _next;
    }
    _hwnd = 0;
    if(and_release)
      release();
  }

  frame* frame::ptr(HWND hwnd)
  {
    return reinterpret_cast<frame*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
  }


  json::value frame::debug(unsigned argc, const json::value* argv)
  {
    for(unsigned n = 0; n < argc; ++n)
    {
      if(n) OutputDebugStringW(L",");
      auto s = argv[n].to_string(CVT_JSON_LITERAL);
      OutputDebugStringW(s.c_str());
    }
    OutputDebugStringW(L"\n");
    return json::value();
  }

  
  //
  //  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
  //
  //  PURPOSE:  Processes messages for the main frame.
  //

  LRESULT CALLBACK frame::wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
  {
  //SCITER integration starts
    BOOL handled = FALSE;
    LRESULT lr = SciterProcND(hWnd,message,wParam,lParam, &handled);
    if( handled )
      return lr;
  //SCITER integration ends
  
    frame* self = ptr(hWnd);
  
	  switch (message)
	  {
	  case WM_DESTROY:
      SetWindowLongPtr(hWnd, GWLP_USERDATA,0);
      self->forget();
      //if( !_first ) // last window closed
		  //   PostQuitMessage(0);
		  break;
    case WM_CLOSE: 
      if(self->on_close())
        break;
    default:
      if(self && self->on_message(message,wParam,lParam,lr))
        return lr;
	  }
    
	  return DefWindowProc(hWnd, message, wParam, lParam);
  }

  json::string frame::get_title()
  {
    TCHAR buf[256] = {0};
    ::GetWindowText(_hwnd, buf, 255);
    return json::string(buf);
  }
  void frame::set_title(const json::string& title)
  {
    ::SetWindowText(_hwnd, title.c_str() );
  }

// main_frame stuff

  main_frame::main_frame(const wchar_t* url)
    :_is_glassy(false) 
  { 
    setup(url); 
  }

  void main_frame::init_window()
  {
    enable_debugger(true);
    frame::init_window();
  }

  void main_frame::init_instance(const dom::element& root)
  {
    if(root.get_style_attribute("background-color") == L"transparent")
       switch_glass(true);
  }


  json::value main_frame::open(json::value url, json::value param)
  {
    if( !url.is_string() )
      return json::value(false);
    sciter::main_frame* wnd = new sciter::main_frame(url.to_string().c_str());
    return json::value(wnd != 0);
  }

  void main_frame::switch_glass(bool on_off)
  {

    typedef HRESULT STDAPICALLTYPE DwmExtendFrameIntoClientArea_t(HWND hWnd, _In_ const MARGINS* pMarInset);

    static DwmExtendFrameIntoClientArea_t* _DwmExtendFrameIntoClientArea = 0;
    static bool load_attempt_has_been_made = false;

    if(!_DwmExtendFrameIntoClientArea) {
      if(load_attempt_has_been_made)
        return;
      load_attempt_has_been_made = true;
      HMODULE hm = LoadLibrary(L"dwmapi.dll");
      if( !hm ) 
        return;
      _DwmExtendFrameIntoClientArea = reinterpret_cast<DwmExtendFrameIntoClientArea_t*>(GetProcAddress(hm,"DwmExtendFrameIntoClientArea"));
      if( !_DwmExtendFrameIntoClientArea )
        return;
    }

    _is_glassy = on_off;
    static MARGINS unlimited_margins = {-1};
    static MARGINS zero_margins = {0};
    HRESULT hr = _DwmExtendFrameIntoClientArea(get_hwnd(),_is_glassy?&unlimited_margins:&zero_margins); // Extend frame across entire window.
    assert(SUCCEEDED(hr));

    ::SciterSetOption(get_hwnd(),SCITER_TRANSPARENT_WINDOW,_is_glassy);
    RECT rc;
    ::GetWindowRect(get_hwnd(), &rc);
    ::SetWindowPos(get_hwnd(), NULL,
                 rc.left, rc.top,
                 rc.right - rc.left, rc.bottom - rc.top,
                 SWP_FRAMECHANGED);    
  }

  json::value main_frame::get_title()
  {
    return json::value(frame::get_title());
  }
  json::value main_frame::set_title(json::value title)
  {
    frame::set_title(title.to_string());
    return json::value();
  }

  json::value main_frame::get_glass()
  {
    return json::value(_is_glassy);
  }
  json::value main_frame::set_glass(json::value on_off)
  {
     json::value pv = get_glass();
     switch_glass(on_off.get(false));
     return pv;
  }
  
  bool main_frame::on_message(UINT message, WPARAM wparam, LPARAM lparam, LRESULT& lresult) 
  {
    return false;
  }

  json::value main_frame::inspector_is_present()
  {
    HWND hwnd = FindWindow( WSTR("H-SMILE-FRAME"), WSTR("Sciter's Inspector"));

    return json::value(hwnd != NULL);
  }


  json::value main_frame::get_native_functor_test()
  {
      // returns native function/functor callable from script

      json::value message = json::value(L"native functor called");

      auto test_functor = [=](unsigned int argc, const json::value* argv ) -> json::value { 
        return message;
      };
/*
      json::value nfv = json::value(test_functor);
      json::value t = nfv.call();
      assert( t == message ); 
*/
      return json::value(test_functor);
  }





}


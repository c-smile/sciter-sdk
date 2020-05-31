// plain-win.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "sciter.h"
#include "sciter-x-dom.hpp"
#include "sciter-x-request.hpp"
#include <process.h>
#include <vector>

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

	  wcex.style			    = 0;//CS_HREDRAW | CS_VREDRAW;
	  wcex.lpfnWndProc	  = wnd_proc;
	  wcex.cbClsExtra		  = 0;
	  wcex.cbWndExtra		  = 0;
	  wcex.hInstance		  = ghInstance;
	  wcex.hIcon			    = LoadIcon(ghInstance, MAKEINTRESOURCE(IDI_SCITER));
	  wcex.hCursor		    = LoadCursor(NULL, IDC_ARROW);
	  wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
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
    asset_add_ref();
  }

  bool frame::setup(const wchar_t* url, HWND parent)
  {
    _hwnd = CreateWindowEx(/* 0x00200000L WS_EX_NOREDIRECTIONBITMAP*/ 0,
       WINDOW_CLASS_NAME, L"sciter-frame", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, parent, NULL, ghInstance, this);
    
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

    /*dom::element frame = root_el.find_first("frame");
    frame.set_style_attribute("transform", L"translate(10dip, 10dip)");
    auto attrval = frame.get_style_attribute("transform");
    attrval = attrval;*/
        
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
      asset_release();
  }

  frame* frame::ptr(HWND hwnd)
  {
    return reinterpret_cast<frame*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
  }


  sciter::value frame::debug(unsigned argc, const sciter::value* argv)
  {
    for(unsigned n = 0; n < argc; ++n)
    {
      if(n) OutputDebugStringW(L",");
      auto s = argv[n].to_string(CVT_JSON_LITERAL);
      OutputDebugStringW(s.c_str());
    }
    OutputDebugStringW(L"\n");
    return sciter::value();
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
    SciterSetOption(get_hwnd(), SCITER_SET_MAIN_WINDOW, TRUE);
  }

  void main_frame::init_window()
  {
    enable_debugger(true);
    frame::init_window();
  }

  void main_frame::init_instance(const dom::element& root)
  {
  }


  sciter::value main_frame::open(sciter::value url, sciter::value param)
  {
    if( !url.is_string() )
      return sciter::value(false);
    sciter::main_frame* wnd = new sciter::main_frame(url.to_string().c_str());
    return sciter::value(wnd != 0);
  }
    
  sciter::value main_frame::get_title()
  {
    return sciter::value(frame::get_title());
  }
  sciter::value main_frame::set_title(sciter::value title)
  {
    frame::set_title(title.to_string());
    return sciter::value();
  }
  
  bool main_frame::on_message(UINT message, WPARAM wparam, LPARAM lparam, LRESULT& lresult) 
  {
    return false;
  }

  sciter::value main_frame::inspector_is_present()
  {
    HWND hwnd = FindWindow( WSTR("H-SMILE-FRAME"), WSTR("Sciter's Inspector"));

    return sciter::value(hwnd != NULL);
  }

  sciter::value main_frame::get_native_functor_test()
  {
      // returns native function/functor callable from script

      sciter::value message = sciter::value(L"native functor called");

      auto test_functor = [=](unsigned int argc, const sciter::value* argv ) -> sciter::value { 
        return message;
      };
/*
      sciter::value nfv = sciter::value(test_functor);
      sciter::value t = nfv.call();
      assert( t == message ); 
*/
      return sciter::value(test_functor);
  }





}


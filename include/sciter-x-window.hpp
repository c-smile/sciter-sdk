/*
 * The Sciter Engine of Terra Informatica Software, Inc.
 * http://sciter.com
 *
 * The code and information provided "as-is" without
 * warranty of any kind, either expressed or implied.
 *
 * (C) 2003-2015, Terra Informatica Software, Inc.
 */

/*
 * sciter::window and sciter::application - high level window wrapper
 * Use these if you just need to create basic Sciter windows.
 * Check /demos/uminimal and /demos/usciter samples.
 */

#pragma once

#ifndef __sciter_x_window_hpp__
#define __sciter_x_window_hpp__

#include "sciter-x-types.h"
#include "sciter-x.h"
#include "sciter-x-dom.h"
#include "sciter-x-api.h"
//#include "sciter-x-threads.h"
#include "sciter-x-dom.hpp"
#include "sciter-x-host-callback.h"
#include "aux-asset.h"
#include "aux-slice.h"
#include <algorithm>

// main UI application routine
int uimain( std::function<int()> run );
// {
//   ... initialization and window creation
//   int r = run();
//   ... optional de-initialization
//   return r;
// }

/**sciter namespace.*/
namespace sciter
{

  namespace application
  {
    const std::vector<sciter::string>& argv();
    HINSTANCE                          hinstance();
  }

  class window : public aux::asset
               , public sciter::host<window>
               , public sciter::event_handler
  {
  public:
    window( UINT creationFlags, RECT frame = RECT() );

    bool is_valid() const { return _hwnd != 0; }

    void collapse(); // minimize
    void expand( bool maximize = false); // show or maximize
    void dismiss(); // delete the window

    bool load( aux::bytes utf8_html, const WCHAR* base_url = 0 );
    bool load( aux::chars utf8_html, const WCHAR* base_url = 0 );
    bool load( const WCHAR* url );

  // sciter::host traits:
    HWINDOW   get_hwnd() { return _hwnd; }
    HINSTANCE get_resource_instance() { return application::hinstance(); }

  protected:
    virtual void detached  (HELEMENT /*he*/ ) override /*sciter::event_handler*/
    {
      // this happens when HWINDOW gets destroyed
      _hwnd = 0; asset::release();
    }

#if defined(WINDOWS)
    virtual LRESULT on_message( HWINDOW hwnd, UINT msg, WPARAM wParam, LPARAM lParam, BOOL& handled );
    static LRESULT SC_CALLBACK msg_delegate(HWINDOW hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID pParam, BOOL* pHandled);
#endif
  private:
     HWINDOW _hwnd;
   };
}

#endif

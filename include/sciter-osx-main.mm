//
//  sciter-osx-main.mm
//  sciter
//
//  Created by andrew on 2014-03-23.
//  Copyright (c) 2014 andrew fedoniouk. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <AppKit/NSApplication.h>

#include "sciter-x-window.hpp"


static std::vector<sciter::string> _argv;

int main(int argc, const char * argv[])
{
  NSApplication * application = [NSApplication sharedApplication];
    
  for( int i = 0; i < argc; ++i ) {
      aux::a2w w(argv[i]);
      _argv.push_back(sciter::string(w.c_str(),w.length()));
  }
    
  auto message_pump = [&]() -> int {
    [application run];
    return 0;
  };
    
  return uimain(message_pump);
   
  //auto message_pump = [&]() -> int {
  //  return NSApplicationMain(argc,argv);
  //};
  //return uimain(message_pump);
}

namespace sciter {
    
  NSView*   nsview(HWINDOW hwnd) { return (__bridge NSView*) hwnd; }
  NSWindow* nswindow(HWINDOW hwnd) { return hwnd ? [nsview(hwnd) window]:nullptr; }
   
  namespace application {
    HINSTANCE hinstance()
    {
      return nullptr; // not used
    }
    
    const std::vector<sciter::string>& argv() {
      return _argv;
    }
    
  }


  bool window::load( aux::bytes utf8_html, const WCHAR* base_url)
  {
     return FALSE != ::SciterLoadHtml(_hwnd,utf8_html.start,utf8_html.length, base_url);
  }
  bool window::load( aux::chars utf8_html, const WCHAR* base_url)
  {
     return FALSE != ::SciterLoadHtml(_hwnd,(LPCBYTE)utf8_html.start,utf8_html.length, base_url);
  }
  bool window::load( const WCHAR* url)
  {
     return FALSE != ::SciterLoadFile(_hwnd,url);
  }

  void window::collapse() {
    if(_hwnd) [nswindow(_hwnd) performMiniaturize:nsview(_hwnd)];
  }
  void window::expand( bool maximize) { 
    if(_hwnd) [nswindow(_hwnd) makeKeyAndOrderFront:nil];
  }    

  void window::dismiss() {
    if(_hwnd) [nswindow(_hwnd) close];
    _hwnd = 0;
  }

  window::window( UINT creationFlags, RECT frame): _hwnd(NULL)
  {
    _hwnd = ::SciterCreateWindow(creationFlags, (frame.right - frame.left) > 0 ? &frame: NULL,NULL,this,NULL);
    if( _hwnd ) {
      add_ref();
      setup_callback();
      sciter::attach_dom_event_handler(get_hwnd(),this);
    }
  }

}
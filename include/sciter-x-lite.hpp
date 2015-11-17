#ifndef __sciter_lite_hpp__
#define __sciter_lite_hpp__

#include "aux-asset.h"
#include "aux-slice.h"
#include "sciter-x.h"
#include "sciter-x-dom.hpp"

namespace sciter
{

  //|
  //| "windowless" sciter engine API
  //|

  class scilite;
  class scilite_host;

  // Create scilite, will use internal bitmap buffer
  BOOL SCAPI SciliteCreate(scilite** out, scilite_host* in);

#ifdef SCILITE_USE_D2D   
  BOOL SCAPI SciliteCreateOnD2DRenderingTarget(scilite** out, scilite_host* in, ID2D1RenderTarget *pRT, ID2D1Factory* pDF, IDWriteFactory* pDWF = 0 );
#endif
#ifdef SCILITE_USE_GDIPLUS   
  BOOL SCAPI SciliteCreateOnGDIPPGraphics(scilite** out, scilite_host* in,Gdiplus::Graphics* pGfx);
#endif
#ifdef SCILITE_USE_COREGRAPHICS   
  BOOL SCAPI SciliteCreateOnGDIPPGraphics(scilite** out, scilite_host* in,CGContextRef pGfx);
#endif
#ifdef SCILITE_USE_CAIRO   
  BOOL SCAPI SciliteCreateOnCairoGraphics(scilite** out, scilite_host* in, cairo_t *cr);
#endif
  
  #define SCILITE_INAME "scilite.sciter.com"
 
  
  //|
  //| that is windowless sciter engine per se
  //|

  class scilite: public aux::iasset 
  {
  public:

    virtual bool     load_html(LPCBYTE html, UINT html_length, LPCWSTR base_url) = 0;
    virtual bool     load_file(LPCWSTR base_url) = 0;

    virtual HELEMENT root_element() = 0;

    virtual bool     document_min_width(int& width);
    virtual bool     document_min_height(int& height, int for_width);
    virtual bool     set_dimensions(int width, int height);

    virtual bool     accept_url_data(LPCWSTR url, LPCBYTE data, UINT data_size, LPVOID request_id) = 0;

    virtual bool     traverse_mouse_event( UINT mouse_cmd, POINT pt, UINT buttons, UINT alt_state ) = 0; 
    virtual bool     traverse_keyboard_event( UINT mouse_cmd, POINT pt, UINT buttons, UINT alt_state ) = 0; 
    virtual bool     traverse_focus_event( UINT focus_cmd ) = 0; 
    virtual bool     traverse_timer_event( UINT_PTR timer_id ) = 0; 

    virtual bool     get_invalid_area( RECT& rc_invalid ) = 0; 
    
  };

  #define SCILITE_HOST_INAME "host.scilite.sciter.com"

  //|
  //| scilite outbound interface
  //|
  class scilite_host: public aux::asset 
  {
  public:
    virtual bool  get_interface(const char* name, iasset** out) 
    { 
      if( aux::streq(name,SCILITE_HOST_INAME) ) { this->add_ref(); *out = this; return true; }
      return scilite_host::get_interface(name,out);
    }

    virtual bool load_data(LPSCN_LOAD_DATA data_rq, bool& discard) = 0;
    virtual bool data_loaded(LPSCN_DATA_LOADED data) = 0;
    virtual bool attach_behavior(LPSCN_ATTACH_BEHAVIOR data) = 0;

    virtual bool set_timer(UINT_PTR tid, UINT elapse_time, bool should_be_precise ) = 0;
    virtual bool set_cursor(UINT cursor_id ) = 0;

    virtual bool start_drawing(RECT rc) = 0;
    virtual bool end_drawing() = 0;

  };

}

#endif
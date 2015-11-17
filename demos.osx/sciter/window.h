//
//  window.h
//  sciter
//
//  Created by andrew@terrainformatica.com on 2014-06-30.
//  Copyright (c) 2014 Andrew Fedoniouk. All rights reserved.
//

#ifndef sciter_window_h
#define sciter_window_h

#include "sciter-x.h"
#include "aux-asset.h"
#include "aux-cvt.h"

bool get_sciter_resource(const char* name, const byte* &data, size_t& data_length);

class window: public aux::asset,
              public sciter::host<window>,
              public sciter::event_handler
{
    HWINDOW hwnd;
public:
    window(LPCWSTR url, RECT frame)
    {
        hwnd = SciterCreateWindow(SW_TITLEBAR | SW_RESIZEABLE | SW_MAIN | SW_ENABLE_DEBUG, &frame, 0,0,0);
        setup_callback(hwnd);
        sciter::attach_dom_event_handler(hwnd, this);
        load_file(url);
        aux::asset::add_ref();
    }
    
    LRESULT on_engine_destroyed() { aux::asset::release(); return 0; }
    
    LRESULT on_load_data(LPSCN_LOAD_DATA pnmld)
    {
        aux::wchars wu = aux::chars_of(pnmld->uri);
        if(wu.like(WSTR("this://app/*"))) {
            // try to get them from archive first
            aux::bytes adata = sciter::archive::instance().get(wu.start+11);
            if(adata.length)
                ::SciterDataReady( pnmld->hwnd, pnmld->uri, adata.start, adata.length);
        }
        //else {
        //    aux::w2utf aurl(wu);
        //    printf("res not found %s\n", aurl.c_str());
        //}
        return LOAD_OK;
    }
    
    bool load_resource_data(LPCWSTR uri, LPCBYTE& pbytes, UINT& nbytes ) {
        aux::wchars wu = aux::chars_of(uri);
        if(wu.like(WSTR("this://app/*"))) {
            // try to get them from archive first
            aux::bytes adata = sciter::archive::instance().get(wu.start+11);
            pbytes = adata.start;
            nbytes = adata.length;
            return true;
        }
        return false;
    }
    
    HWINDOW   get_hwnd() const { return hwnd; }
    HINSTANCE get_resource_instance() const { return 0; /*not used on this platform*/ }
    
    BEGIN_FUNCTION_MAP
      FUNCTION_0("glass", get_glass);
      FUNCTION_1("glass", set_glass);
    END_FUNCTION_MAP
    
    sciter::value get_glass();
    sciter::value set_glass(sciter::value on_off);

};


#endif

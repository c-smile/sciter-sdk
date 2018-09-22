//
//  app.cpp
//  sciter
//
//  Created by andrew on 2014-04-06.
//  Copyright (c) 2014 andrew fedoniouk. All rights reserved.
//

#include <stdio.h>

#include "sciter-x.h"

#include "resources.cpp"

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

UINT SciterViewCallback( LPSCITER_CALLBACK_NOTIFICATION pns, LPVOID callbackParam )
{
    switch(pns->code) {
        case SC_LOAD_DATA:
        {
            LPSCN_LOAD_DATA pc = LPSCN_LOAD_DATA(pns);
            load_resource_data(pc->uri,pc->outData,pc->outDataSize);
            return LOAD_OK;
        }
        case SC_DATA_LOADED: break;
        case SC_ATTACH_BEHAVIOR: break;
    }
    return 0;
}


extern "C" HWINDOW startup()
{
    RECT frame;
    frame.top = 100;
    frame.left = 100;
    frame.right = 100 + 500 + 1;
    frame.bottom = 100 + 500 + 1;
    
    sciter::archive::instance().open(aux::elements_of(resources)); // bind resources[] (defined in "resources.cpp") with the archive
    
    HWINDOW hw = SciterCreateWindow(SW_MAIN | SW_ALPHA | SW_TOOL, &frame, 0,0,0);
    SciterSetCallback(hw, &SciterViewCallback, nullptr);
    SciterLoadFile(hw, WSTR("this://app/default.htm"));
    
    return hw;
}

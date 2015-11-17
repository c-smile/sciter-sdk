//
//  app.cpp
//  sciter
//
//  Created by andrew on 2014-04-06.
//  Copyright (c) 2014 andrew fedoniouk. All rights reserved.
//

#include <stdio.h>

#include "sciter-x.h"

bool get_layered_resource(const char* name, const byte* &data, size_t& data_length);

UINT SciterViewCallback( LPSCITER_CALLBACK_NOTIFICATION pns, LPVOID callbackParam )
{
    switch(pns->code) {
        case SC_LOAD_DATA:
        {
            LPSCN_LOAD_DATA pc = LPSCN_LOAD_DATA(pns);
            aux::w2a auri(pc->uri);
            if( auri().like( "res:*" ) ) {
                LPCBYTE pdata; size_t data_length;
                if( get_layered_resource( auri.c_str() + 4, pdata, data_length ) )
                {
                    pc->outData = pdata;
                    pc->outDataSize = UINT(data_length);
                    return LOAD_OK;
                }
            }
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
    
    HWINDOW hw = SciterCreateWindow(SW_MAIN | SW_ALPHA | SW_TOOL, &frame, 0,0,0);
    SciterSetCallback(hw, &SciterViewCallback, nullptr);
    SciterLoadFile(hw, WSTR("res:default.htm"));
    
    return hw;
}

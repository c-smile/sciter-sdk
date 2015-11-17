//
//  window.mm
//  sciter
//
//  Created by andrew on 2014-06-30.
//  Copyright (c) 2014 andrew fedoniouk. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include "window.h"

NSView*   get_nsview(window* w)   { return static_cast<NSView*>(w->get_hwnd()); }
NSWindow* get_nswindow(window* w) { return [get_nsview(w) window]; }

sciter::value window::get_glass() {
    NSWindow* nsw = get_nswindow(this);
    bool on = [nsw isOpaque] != YES;
    return sciter::value(on);
}
sciter::value window::set_glass(sciter::value on_off)
{
    NSWindow* nsw = get_nswindow(this);
    if( on_off.get(false) )
    {
        [nsw setOpaque:NO];
        NSColor* backgroundColor = [nsw backgroundColor];
        backgroundColor = [backgroundColor colorWithAlphaComponent:0.5];
        [nsw setBackgroundColor:backgroundColor];
    }
    else
    {
        [nsw setOpaque:YES];
    }
    
    sciter::value vars;
    vars.set_item(sciter::value("on-glass"), on_off);
    SciterSetMediaVars(get_hwnd(), &vars);
    
    return on_off;
}


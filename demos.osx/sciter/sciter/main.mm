//
//  main.m
//  sciter
//
//  Created by andrew on 2014-03-23.
//  Copyright (c) 2014 andrew fedoniouk. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <AppKit/NSApplication.h>

#include "window.h"
#include "../resources.cpp"

extern NSView*   get_nsview(window* w);
extern NSWindow* get_nswindow(window* w);


int main(int argc, const char * argv[])
{
    //::SciterSetOption(NULL,SCITER_SET_GFX_LAYER,1); // use this to force the engine to use CoreGraphics backend.
    
    // enable features to be used from script
    SciterSetOption(NULL, SCITER_SET_SCRIPT_RUNTIME_FEATURES,
                    ALLOW_FILE_IO |
                    ALLOW_SOCKET_IO |
                    ALLOW_EVAL |
                    ALLOW_SYSINFO );
    
    
    NSApplication * application = [NSApplication sharedApplication];
    NSArray *tl;
    [[NSBundle mainBundle] loadNibNamed:@"MainMenu" owner:application topLevelObjects:&tl];
    
    sciter::archive::instance().open(aux::elements_of(sciter_resources)); // bind resources[] (defined in "resources.cpp") with the archive
    
    RECT frame;
    frame.top = 100;
    frame.left = 100;
    frame.right = 100 + 800;
    frame.bottom = 100 + 600;
    
    aux::asset_ptr<window> wnd = new window(WSTR("this://app/default.htm"),frame);
    
    //sciter::debug_output debug_console(wnd->get_hwnd());
    
    [get_nswindow(wnd) makeKeyAndOrderFront: nil];
    
	[application run];
   
}

//
//  main.m
//  layered
//
//  Created by andrew on 2014-04-17.
//  Copyright (c) 2014 andrew fedoniouk. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <AppKit/NSApplication.h>

void* startup();

int main(int argc, const char * argv[])
{
    //return NSApplicationMain(argc, argv);
    //load_sciter_dylib();
    
    NSApplication * application = [NSApplication sharedApplication];
    
    NSView* ns = (NSView*) startup();
    
    [[ns window] makeKeyAndOrderFront: nil];
    
	[application run];
    
}

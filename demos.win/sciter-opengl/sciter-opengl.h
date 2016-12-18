#pragma once

#include "resource.h"

#include "sciter-x.h"
#include "sciter-x-behavior.h"
#include "sciter-x-threads.h"

//#include <thread>

#if defined(WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/GL.h>
#include <gl/GLu.h>
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "Glu32.lib")
#elif defined(OSX)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/CGLTypes.h>
#elif defined(LINUX)
#include <gtk/gtk.h>
#include <GL/gl.h>
#include <GL/glx.h>
#endif

struct  context {
	HWND  hwnd;
	HGLRC mainGLRC;
	bool  done;
	sciter::sync::event event_draw;
	sciter::sync::event event_draw_complete;
};

void opengl_thread(context* ctx);


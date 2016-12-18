#include "stdafx.h"
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

void opengl_thread(context* ctx)
{

  HDC    hdc = GetDC(ctx->hwnd);
  HGLRC  hglrc = wglCreateContext(hdc); ;

  wglShareLists(ctx->mainGLRC,hglrc);

  // make it the calling thread's current rendering context 
  wglMakeCurrent(hdc, hglrc);

  //Set up the orthographic projection so that coordinates (0, 0) are in the top left
  //and the minimum and maximum depth is -10 and 10. To enable depth just put in
  //glEnable(GL_DEPTH_TEST)
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, 640, 480, 0, -10, 10);

  //Back to the modelview so we can draw stuff 
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear the screen and depth buffer

  for(;;) {

    ctx->event_draw.wait();
    if(ctx->done)
     break;

    wglMakeCurrent(hdc, hglrc); // ?

    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();         //Make sure our transformations don't affect any other transformations in other code
    glTranslatef(0, 0, 10);  //Translate rectangle to its assigned x and y position
                            //Put other transformations here
    glBegin(GL_QUADS);      //We want to draw a quad, i.e. shape with four sides
    glColor3f(1, 0, 0);     //Set the colour to red 
    glVertex2f(0, 0);       //Draw the four corners of the rectangle
    glVertex2f(0, 480);
    glVertex2f(640,480);
    glVertex2f(640, 0);
    glEnd();
    glPopMatrix();

    glFlush();

    ctx->event_draw_complete.signal();
  
  }

  // make the rendering context not current  
  wglMakeCurrent(NULL, NULL);
  // delete the rendering context  
  wglDeleteContext(hglrc);

  ReleaseDC(ctx->hwnd,hdc);

  //ctx->event_draw_complete.signal();
}



namespace sciter
{
/*
BEHAVIOR: native-clock
  - draws content layer using sciter-x-graphics.hpp primitives.

SAMPLE:
   See: samples/behaviors/native-drawing.htm
*/

struct molehill_opengl: public event_handler
{
    context* ctx;

    // ctor
    molehill_opengl(): ctx(nullptr) {}
    virtual ~molehill_opengl() {}

    virtual bool subscription( HELEMENT he, UINT& event_groups )
    {
      event_groups = HANDLE_DRAW;   // it does drawing
      return true;
    }

    virtual void attached  (HELEMENT he ) 
    {
      ctx = new context();
      ctx->hwnd = dom::element(he).get_element_hwnd(true);
      ctx->mainGLRC = wglGetCurrentContext();
      sciter::thread(opengl_thread,ctx);
    }
    virtual void detached  (HELEMENT he ) { 
      ctx->done = true;
      ctx->event_draw.signal();
      //ctx->event_draw_complete.wait();
      //delete ctx;
      delete this; 
    }

    virtual bool handle_draw   (HELEMENT he, DRAW_PARAMS& params ) 
    {
      if( params.cmd != DRAW_CONTENT ) return false; // drawing only content layer
       
      ctx->event_draw.signal();
      //ctx->event_draw_complete.wait();

      //return false;

      return true; // done drawing
    
    }

};

struct molehill_opengl_factory: public behavior_factory {

  molehill_opengl_factory(): behavior_factory("molehill-opengl") { }

  // the only behavior_factory method:
  virtual event_handler* create(HELEMENT he) {
    return new molehill_opengl();
  }

};

// instantiating and attaching it to the global list
molehill_opengl_factory molehill_opengl_factory_instance;


}

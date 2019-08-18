
#include "stdafx.h"
#include "sciter-opengl.h"

extern HWND  hWnd;
extern HGLRC hglrc_main;

static HGLRC  hglrc = 0;
static float  theta = 0;
static HDC    hdc = 0;
static DWORD  ticks = 0;

void init_3d() {
  ticks = GetTickCount();
  hdc = GetDC(hWnd);
  hglrc = wglCreateContext(hdc);
  theta = 0;
  wglShareLists(hglrc_main, hglrc);
  //ReleaseDC(hwnd, hdc);
}

void stop_3d() {
  wglMakeCurrent(NULL, NULL);
  // delete the rendering context  
  wglDeleteContext(hglrc);
  ReleaseDC(hWnd, hdc);
}

void draw_3d()
{
	// make it the calling thread's current rendering context 
	wglMakeCurrent(hdc, hglrc);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear the screen and depth buffer

	glPushMatrix();
	glRotatef(theta, 0.0f, 0.0f, 1.0f);
	glBegin(GL_TRIANGLES);
	glColor3f(1.0f, 0.0f, 0.0f); glVertex2f(0.0f, 1.0f);
	glColor3f(0.0f, 1.0f, 0.0f); glVertex2f(0.87f, -0.5f);
	glColor3f(0.0f, 0.0f, 1.0f); glVertex2f(-0.87f, -0.5f);
	glEnd();
	glPopMatrix();

	glFlush();

	DWORD nticks = GetTickCount();

	theta += (nticks - ticks) / 100.0f;
	ticks = nticks;

  wglMakeCurrent(hdc, hglrc_main);
 
}



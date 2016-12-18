
#include "stdafx.h"
#include "sciter-opengl.h"


void opengl_thread(context* ctx)
{

	HDC    hdc = GetDC(ctx->hwnd);
	HGLRC  hglrc = wglCreateContext(hdc);
	float  theta = 0;

	wglShareLists(ctx->mainGLRC, hglrc);

	// make it the calling thread's current rendering context 
	wglMakeCurrent(hdc, hglrc);

	DWORD ticks = GetTickCount();

	for (;;) {

		ctx->event_draw.wait();
		if (ctx->done)
			break;

		wglMakeCurrent(hdc, hglrc); // ?

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

		ctx->event_draw_complete.signal();

		DWORD nticks = GetTickCount();

		theta += (nticks - ticks) / 100.0f;
		ticks = nticks;

	}

	// make the rendering context not current  
	wglMakeCurrent(NULL, NULL);
	// delete the rendering context  
	wglDeleteContext(hglrc);

	ReleaseDC(ctx->hwnd, hdc);

	//ctx->event_draw_complete.signal();
}

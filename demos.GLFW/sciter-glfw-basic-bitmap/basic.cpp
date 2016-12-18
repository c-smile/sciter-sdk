//========================================================================
// Simple GLFW example by Camilla Berglund <elmindreda@glfw.org>
// 
// Modified for Sciter integration purposes by Andrew Fedoniouk <andrew@sciter.com>
//========================================================================
//! [code]

// SCITER {
#include "sciter-x.h"
#include "sciter-x-dom.hpp"
// } SCITER

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "linmath.h"

#include <stdlib.h>
#include <stdio.h>

#include "glbitmap.h"

static struct
{
    float x, y;
    float r, g, b;
} vertices[3] =
{
    { -0.6f, -0.4f, 1.f, 0.f, 0.f },
    {  0.6f, -0.4f, 0.f, 1.f, 0.f },
    {   0.f,  0.6f, 0.f, 0.f, 1.f }
};

double speed_factor = 1.0;
float color_offsets[3][3] = { {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };

static const char* vertex_shader_text =
"uniform mat4 MVP;\n"
"uniform vec3 vcol_offsets[3] = {{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}};\n"
"attribute vec3 vCol;\n"
"attribute vec2 vPos;\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    color = vCol + vcol_offsets[(gl_VertexID % 3)];\n"
"}\n";

static const char* fragment_shader_text =
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(color, 1.0);\n"
"}\n";

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

HWINDOW hwnd(GLFWwindow *window) {
#ifdef WINDOWS
   return glfwGetWin32Window(window);
#endif
}

namespace sciter {
  WNDPROC former = nullptr;

  LRESULT CALLBACK SciterWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
  {
     BOOL handled = FALSE;
     if( uMsg != WM_PAINT ) {
       LRESULT lr = SciterProcND(hwnd, uMsg, wParam, lParam, &handled);
       if( handled ) return lr;
     }
     return CallWindowProc(former, hwnd, uMsg, wParam, lParam);
  }

  struct dom_events_handler : sciter::event_handler
  {

    BEGIN_FUNCTION_MAP
      FUNCTION_1("getCornerColor"  , getCornerColor)
      FUNCTION_4("setCornerColor"  , setCornerColor)
      FUNCTION_0("getRotationSpeed", getRotationSpeed)
      FUNCTION_1("setRotationSpeed", setRotationSpeed)
    END_FUNCTION_MAP

    sciter::value getCornerColor(sciter::value n) {
       int i = n.get<int>();
       assert( i >= 0 && i < 3 );
       sciter::value result[3] = { sciter::value(vertices[i].r + color_offsets[i][0]), sciter::value(vertices[i].g + color_offsets[i][1] ), sciter::value(vertices[i].b + color_offsets[i][2] ) };
       return sciter::value( result, 3 );
    }
    sciter::value setCornerColor(sciter::value n, sciter::value r, sciter::value g, sciter::value b ) {
      int i = n.get<int>();
      assert(i >= 0 && i < 3);
	  color_offsets[i][0] = r.get<double>() - vertices[i].r;
	  color_offsets[i][1] = g.get<double>() - vertices[i].g;
	  color_offsets[i][2] = b.get<double>() - vertices[i].b;
      return sciter::value(); // undefined
    }

    sciter::value getRotationSpeed() {
      return sciter::value(speed_factor); // undefined
    }
    sciter::value setRotationSpeed(sciter::value factor) {
      speed_factor = factor.get<double>();
      return sciter::value(); // undefined
    }

  } the_dom_events_handler;
  
  bool attach(GLFWwindow *window) {

    // enable inspector.exe on this window
    SciterSetOption(NULL, SCITER_SET_DEBUG_MODE, TRUE);
    SciterSetOption(NULL, SCITER_SET_GFX_LAYER, GFX_LAYER_SKIA);

    HWINDOW hw = hwnd(window);
    BOOL dummy;
    // subclass the window
    former = (WNDPROC)::GetWindowLongPtr(hw, GWLP_WNDPROC);
    ::SetWindowLongPtr(hw, GWLP_WNDPROC, (LONG_PTR)&SciterWindowProc);
    SciterProcND(hw,WM_CREATE,0,0,&dummy);
    sciter::attach_dom_event_handler(hw, &the_dom_events_handler);
    return true;
  }


}

//////////////////////////////////////////
GLBitmap gBackScreenBitmap;
VOID SC_CALLBACK BackElementBitmapReceiver( LPCBYTE bgra, INT x, INT y, UINT width, UINT height, LPVOID param )
{
  gBackScreenBitmap.Update( width, height, bgra);
}

GLBitmap gForeScreenBitmap;
VOID SC_CALLBACK ForeElementBitmapReceiver(LPCBYTE bgra, INT x, INT y, UINT width, UINT height, LPVOID param)
{
  gForeScreenBitmap.Update(width, height, bgra);
}


//ADD CALLBACK HERE: gFullScreenBitmap.Update( width, height, bitmapData.data( ) );
//////////////////////////////////////////

int main(int argc, char *argv[])
{
#ifdef WINDOWS
    OleInitialize(NULL);
#endif
    GLFWwindow* window;
    GLuint vertex_array, vertex_buffer, vertex_shader, fragment_shader, program;
    GLint mvp_location, vpos_location, vcol_location, vcol_offsets_location;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

	glfwSetKeyCallback( window, key_callback );

	glfwMakeContextCurrent( window );
	gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress );
	glfwSwapInterval( 1 );

    // SCITER 

    sciter::dom::element fore_layer;
    sciter::dom::element back_layer;

    if( sciter::attach(window) ) {
        SciterLoadFile(hwnd(window), L"sciter-glfw-basic-facade.htm" );

        sciter::dom::element root = sciter::dom::element::root_element(hwnd(window));

        back_layer = root.find_first("section#back-layer");
        fore_layer = root.find_first("section#fore-layer");
        assert(back_layer && fore_layer);
    }
    // SCITER


    // NOTE: OpenGL error checks have been omitted for brevity

	  glGenVertexArrays( 1, &vertex_array );
	  glBindVertexArray( vertex_array );

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    mvp_location = glGetUniformLocation(program, "MVP");
	  vcol_offsets_location = glGetUniformLocation( program, "vcol_offsets[0]" );
	  vpos_location = glGetAttribLocation(program, "vPos");
    vcol_location = glGetAttribLocation(program, "vCol");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 5, (void*) 0);
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 5, (void*) (sizeof(float) * 2));
	glBindVertexArray( 0 );

	double prev_time = glfwGetTime( );

	float rotation = 0.0f;

	//////////////////////////////////////////
	//ADD CALLBACK HERE: ElementBitmapReceiver (currently full screen)
	//////////////////////////////////////////

  GLBitmap back_bitmap;
  GLBitmap fore_bitmap;

    while (!glfwWindowShouldClose(window))
    {
		double curr_time = glfwGetTime( );
		float delta_time = (float)(curr_time - prev_time);
		prev_time = curr_time;

        float ratio;
        int width, height;
        mat4x4 m, p, mvp;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;

        glViewport(0, 0, width, height);
		    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// SCITER 
		{
      SCITER_X_MSG_PAINT pc(back_layer,FALSE);
      pc.targetType = SPT_RECEIVER;
      pc.target.receiver.callback = BackElementBitmapReceiver;
      pc.target.receiver.param = 0;
      SciterProcX(hwnd(window), pc);
		}
		// SCITER
    // BITMAP DRAW ON TOP
    {
      glEnable(GL_BLEND);
      glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
      GLDrawScreenQuad(gBackScreenBitmap._texture);
    }


		rotation += delta_time * speed_factor;

        mat4x4_identity(m);
        mat4x4_rotate_Z(m, m, rotation);
        mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        mat4x4_mul(mvp, p, m);

		glPushAttrib( unsigned( -1 ) );
		glPushClientAttrib( unsigned( -1 ) );

		glDisable( GL_BLEND );

        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
		glUniform3fv( vcol_offsets_location, 3, (const GLfloat*)color_offsets );

		glBindVertexArray( vertex_array );
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray( 0 );

		glPopClientAttrib( );
		glPopAttrib( );

    // SCITER 
    {
      SCITER_X_MSG_PAINT pc(fore_layer, TRUE);
      pc.targetType = SPT_RECEIVER;
      pc.target.receiver.callback = ForeElementBitmapReceiver;
      pc.target.receiver.param = 0;
      SciterProcX(hwnd(window), pc);
    }

		// BITMAP DRAW ON TOP
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    GLDrawScreenQuad(gForeScreenBitmap._texture);

        glfwSwapBuffers(window);

		double pollevents_delta_time = glfwGetTime( );
        glfwPollEvents();
		pollevents_delta_time = glfwGetTime( ) - pollevents_delta_time;
		prev_time += pollevents_delta_time;
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

//! [code]

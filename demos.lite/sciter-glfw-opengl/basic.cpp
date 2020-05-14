//========================================================================
// Simple GLFW example by Camilla Berglund <elmindreda@glfw.org>
// 
// Modified for Sciter integration purposes by Andrew Fedoniouk <andrew@sciter.com>
//========================================================================
//! [code]

// SCITER {
#include "sciter-x.h"
#include "sciter-x-behavior.h"
#include "sciter-x-dom.hpp"
// } SCITER

//#ifdef LINUX
//  #define GLFW_INCLUDE_ES2
//  #include <GLFW/glfw3.h>
//#else
  #include <glad/glad.h>  
  #include <GLFW/glfw3.h>
  //#include <GLFW/glfw3native.h> 
  #include "linmath.h"
//#endif  


#include <stdlib.h>
#include <stdio.h>

#include "aux-fs.h"

#include "../facade-resources.cpp"

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void char_callback(GLFWwindow* window, unsigned int codepoint);
static void focus_callback(GLFWwindow* window, int got_focus);
static void mouse_button_callback(GLFWwindow * window, int button, int acction, int modifiers);
static void mouse_move_callback(GLFWwindow * window, double x, double y);
static void mouse_enter_leave_callback(GLFWwindow *, int enter);
static void mouse_wheel_callback(GLFWwindow * window, double dx, double dy);

static UINT SC_CALLBACK handle_notification(LPSCITER_CALLBACK_NOTIFICATION pnm, LPVOID callbackParam);

static bool sciter_needs_drawing = true;

int main(int argc, char *argv[])
{
    SciterSetOption(NULL, SCITER_SET_UX_THEMING, TRUE);

    // these two calls are optional, used here to enable communication with inspector.exe (CTRL+SHIFT+I with running inspector) 
    SciterSetOption(NULL, SCITER_SET_SCRIPT_RUNTIME_FEATURES,
      ALLOW_FILE_IO |
      ALLOW_SOCKET_IO |
      ALLOW_EVAL |
      ALLOW_SYSINFO);
    SciterSetOption(NULL, SCITER_SET_DEBUG_MODE, TRUE);
    
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(600, 900, "Lite OpenGL example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // setup event callbacks:
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_move_callback);
    glfwSetCursorEnterCallback(window, mouse_enter_leave_callback);
    glfwSetScrollCallback(window, mouse_wheel_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCharCallback(window, char_callback);
    glfwSetWindowFocusCallback(window, focus_callback);
    
	  glfwMakeContextCurrent( window );
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    int used_width = 0, used_height = 0;

    // SCITER+
      // create the engine and associate it with window:
      SciterProcX(window, SCITER_X_MSG_CREATE(GFX_LAYER_SKIA_OPENGL,FALSE));
      // set device resoultion ( pixels per inch )
      float xscale, yscale;
      glfwGetWindowContentScale(window, &xscale, &yscale);
      SciterProcX(window, SCITER_X_MSG_RESOLUTION(UINT(96 * xscale)));
      // set surface size
      glfwGetFramebufferSize(window, &used_width, &used_height);
      SciterProcX(window, SCITER_X_MSG_SIZE(used_width, used_height));

      SciterSetCallback(window, handle_notification, NULL);

      // load file:

      sciter::archive::instance().open(aux::elements_of(resources)); // bind resources[] (defined in "facade-resources.cpp") with the archive
      SciterLoadFile(window, WSTR("this://app/main.htm"));

    // SCITER-

    // NOTE: OpenGL error checks have been omitted for brevity

    while (!glfwWindowShouldClose(window))
    {
      UINT ticks = UINT(glfwGetTime() * 1000); // in milliseconds
      // give sciter a chance to process animations, timers and other timed things
      SciterProcX(window, SCITER_X_MSG_HEARTBIT(ticks));
      
      int width, height;
      glfwGetFramebufferSize(window, &width, &height);
      if (width != used_width || height != used_height)
        SciterProcX(window, SCITER_X_MSG_SIZE(used_width = width, used_height = height));

      if (sciter_needs_drawing) {
        sciter_needs_drawing = false;
        glViewport(0, 0, width, height);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // SCITER - render Sciter using current OpenGL context
        SciterProcX(window, SCITER_X_MSG_PAINT());

        glfwSwapBuffers(window);
      }

      //glfwWaitEventsTimeout(0.016); // 60 FPS
      glfwPollEvents();
    }

    SciterProcX(window, SCITER_X_MSG_DESTROY());

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

// SCITER+

static MOUSE_BUTTONS mbutton = MOUSE_BUTTONS(0);

static void mouse_button_callback(GLFWwindow * window, int button, int action, int modifiers)
{
  MOUSE_EVENTS  me = action == GLFW_PRESS ? MOUSE_DOWN : MOUSE_UP;
  switch (button) {
    default:
    case GLFW_MOUSE_BUTTON_1: mbutton = MAIN_MOUSE_BUTTON; break;
    case GLFW_MOUSE_BUTTON_2: mbutton = PROP_MOUSE_BUTTON; break;
    case GLFW_MOUSE_BUTTON_3: mbutton = MIDDLE_MOUSE_BUTTON; break;
  }
  //KEYBOARD_STATES 
  UINT ks = 0;
  if (modifiers & GLFW_MOD_SHIFT) ks |= SHIFT_KEY_PRESSED;
  if (modifiers & GLFW_MOD_CONTROL) ks |= CONTROL_KEY_PRESSED;
  if (modifiers & GLFW_MOD_ALT) ks |= ALT_KEY_PRESSED;

  double x, y; glfwGetCursorPos(window, &x, &y);
  POINT pos = {int(x),int(y)};
  
  SciterProcX(window, SCITER_X_MSG_MOUSE(me, mbutton, KEYBOARD_STATES(ks),pos));
  if (me == MOUSE_UP)
    mbutton = MOUSE_BUTTONS(0);
}

static void mouse_move_callback(GLFWwindow * window, double x, double y)
{
  MOUSE_EVENTS  me = MOUSE_MOVE;
  POINT pos = { int(x),int(y) };
  SciterProcX(window, SCITER_X_MSG_MOUSE(me, mbutton, KEYBOARD_STATES(0), pos));
}

static void mouse_enter_leave_callback(GLFWwindow * window, int enter) {
  MOUSE_EVENTS  me = enter ? MOUSE_ENTER : MOUSE_LEAVE;
  UINT ks = 0;
  double x, y; glfwGetCursorPos(window, &x, &y);
  POINT pos = { int(x),int(y) };
  SciterProcX(window, SCITER_X_MSG_MOUSE(me, MOUSE_BUTTONS(0), KEYBOARD_STATES(ks), pos));
}

static void mouse_wheel_callback(GLFWwindow * window, double dx, double dy)
{
  MOUSE_EVENTS  me = MOUSE_WHEEL;
  double x, y; glfwGetCursorPos(window, &x, &y);
  POINT pos = { int(x),int(y) };
  dx *= 16;
  dy *= 16;
  UINT  deltas = ((short)dx) << 16 | (short)dy;
  SciterProcX(window, SCITER_X_MSG_MOUSE(me, MOUSE_BUTTONS(deltas), KEYBOARD_STATES(0), pos));
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  KEY_EVENTS me = GLFW_RELEASE == action ? KEY_UP : KEY_DOWN;
  UINT ks = 0;
  if (mods & GLFW_MOD_SHIFT) ks |= SHIFT_KEY_PRESSED;
  if (mods & GLFW_MOD_CONTROL) ks |= CONTROL_KEY_PRESSED;
  if (mods & GLFW_MOD_ALT) ks |= ALT_KEY_PRESSED;
  SciterProcX(window, SCITER_X_MSG_KEY(me, UINT(key), KEYBOARD_STATES(ks)));
}

static void char_callback(GLFWwindow* window, unsigned int codepoint)
{
  KEY_EVENTS me = KEY_CHAR;
  SciterProcX(window, SCITER_X_MSG_KEY(me, codepoint, KEYBOARD_STATES(0)));
}

static void focus_callback(GLFWwindow* window, int got_focus)
{
  SciterProcX(window, SCITER_X_MSG_FOCUS(!!got_focus));
}



UINT on_load_data(LPSCN_LOAD_DATA pnmld) {

  // your custom loader is here

  LPCBYTE pb = 0; UINT cb = 0;
  aux::wchars wu = aux::chars_of(pnmld->uri);

  if (wu.like(WSTR("this://app/*"))) {
    // try to get them from archive first
    aux::bytes adata = sciter::archive::instance().get(wu.start + 11);
    if (adata.length)
      ::SciterDataReady(pnmld->hwnd, pnmld->uri, adata.start, adata.length);
  }
  return LOAD_OK;
}

UINT on_data_loaded(LPSCN_DATA_LOADED pnm) { return 0; }

UINT attach_behavior(LPSCN_ATTACH_BEHAVIOR lpab) {
  // attach native behaviors (if we have any)
  sciter::event_handler *pb = sciter::behavior_factory::create(lpab->behaviorName, lpab->element);
  if (pb) {
    lpab->elementTag  = pb;
    lpab->elementProc = sciter::event_handler::element_proc;
    return 1;
  }
  return 0;
}

UINT on_invalidate_rect(LPSCN_INVALIDATE_RECT pnm) {
  sciter_needs_drawing = true;
  //glfwPostEmptyEvent();
  return 0;
}

// notifiaction cracker
UINT SC_CALLBACK handle_notification(LPSCITER_CALLBACK_NOTIFICATION pnm, LPVOID callbackParam)
{
  // Crack and call appropriate method
  // here are all notifiactions
  switch (pnm->code) {
    case SC_LOAD_DATA: return on_load_data((LPSCN_LOAD_DATA)pnm);
    case SC_DATA_LOADED: return on_data_loaded((LPSCN_DATA_LOADED)pnm);
    case SC_ATTACH_BEHAVIOR: return attach_behavior((LPSCN_ATTACH_BEHAVIOR)pnm);
    case SC_INVALIDATE_RECT: return on_invalidate_rect((LPSCN_INVALIDATE_RECT)pnm);
    case SC_ENGINE_DESTROYED: break;
  }
  return 0;
}



// SCITER-



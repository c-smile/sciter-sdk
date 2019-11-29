
//|
//|
//| Copyright (c) 2019
//| Andrew Fedoniouk - andrew@terrainformatica.com
//|
//| Sciter.Lite <-> Android bridge: glues Android Native Actrivity with
//Sciter.Lite
//|
//|

#ifndef _SCITER_ANDROID_BRIDGE_H
#define _SCITER_ANDROID_BRIDGE_H

#include "sciter-x.h"
#include "android_native_app_glue.h"

#define LOGI(...)                                                              \
  ((void)__android_log_print(ANDROID_LOG_INFO, "Sciter.NativeActivity",        \
                             __VA_ARGS__))
#define LOGW(...)                                                              \
  ((void)__android_log_print(ANDROID_LOG_WARN, "Sciter.NativeActivity",        \
                             __VA_ARGS__))

/**
 * Our saved state data.
 */
struct saved_state {
  float   angle;
  int32_t x;
  int32_t y;
};

/**
 * Shared state for our app.
 */
struct engine {
  struct android_app *app = nullptr;

  ASensorManager *   sensorManager       = nullptr;
  const ASensor *    accelerometerSensor = nullptr;
  ASensorEventQueue *sensorEventQueue    = nullptr;

  bool               animating = false;
  EGLDisplay         display   = nullptr;
  EGLSurface         surface   = nullptr;
  EGLContext         context   = nullptr;
  int32_t            width     = 0;
  int32_t            height    = 0;
  struct saved_state state;
  uint32_t           keyboardMode = 0; 


};

/**
 * Initialize Sciter engine in this context
 */

bool engine_init(struct engine *engine);

/**
 * Initialize an EGL context for the current display.
 */
int engine_init_display(struct engine *engine);

/**
 * Just draw the current frame on the display.
 */
void engine_draw_frame(struct engine *engine);

/**
 * Tear down the EGL context currently associated with the display.
 */
void engine_term_display(struct engine *engine);

/**
 * Process the next input event.
 */
int32_t engine_handle_input(struct android_app *app, AInputEvent *event);

/**
 * Process the next main command.
 */
void engine_handle_cmd(struct android_app *app, int32_t cmd);

#endif

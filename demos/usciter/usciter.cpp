
#include "sciter-x-window.hpp"
#include "behaviors/behavior_video_generator.cpp"

// native API demo

// native functions exposed to script

static int native_get_element_uid(sciter::value vel) {
  sciter::dom::element el = (HELEMENT)vel.get_object_data();
  return (int)el.get_element_uid();
}


static sciter::value native_api() {

  sciter::value api_map;

  api_map.set_item(sciter::value("getElementUid"), sciter::vfunc( native_get_element_uid ) );
  return api_map;

  /* the above returns this:
    return {
      getElementUid(): {native_get_element_uid}
    }
  */

}

class frame: public sciter::window {
public:
  frame() : window(SW_TITLEBAR | SW_RESIZEABLE | SW_CONTROLS | SW_MAIN | SW_GLASSY | SW_ENABLE_DEBUG) {}

  BEGIN_FUNCTION_MAP
    FUNCTION_0("nativeApi", native_api);
  END_FUNCTION_MAP

};

#include "resources.cpp"

int uimain(std::function<int()> run ) {

  //sciter::debug_output_console console; - uncomment it if you will need console window

  sciter::archive::instance().open(aux::elements_of(resources)); // bind resources[] (defined in "resources.cpp") with the archive

  frame *pwin = new frame();

  // note: this:://app URL is dedicated to the sciter::archive content associated with the application
  pwin->load( WSTR("this://app/default.htm") );

  pwin->expand();

  return run();

}


#include "sciter-x-window.hpp"
#include "sciter-x-graphics.hpp"
//#include "behaviors/behavior_video_generator.cpp"

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

static sciter::value test_image_access(sciter::value vimg)
{
  sciter::image img = sciter::image::from(vimg); /// failed in this code
  UINT w, h;
  img.dimensions(w,h);
  sciter::bytes_writer bw;
  img.save(bw, SCITER_IMAGE_ENCODING_RAW);
  return sciter::value();
}

static sciter::value test_image_generation()
{
  BYTE pixmap[10 * 10 * 4];
  memset(pixmap, 0, sizeof(pixmap));
  // image data
  for (int i = 0; i < 10 * 10 * 4; i += 4)
  {
    pixmap[i] = i / 4;
    pixmap[i + 1] = 255 - i / 4;
    pixmap[i + 2] = 255;
    pixmap[i + 3] = 255;
  }
  sciter::image img = sciter::image::create(10, 10, true, pixmap);
  return img.to_value();
}

class frame: public sciter::window {
public:
  frame() : window(SW_TITLEBAR | SW_RESIZEABLE | SW_CONTROLS | SW_MAIN | SW_ENABLE_DEBUG) {}

  BEGIN_FUNCTION_MAP
    FUNCTION_0("nativeApi", native_api);
    FUNCTION_1("testImageAccess", test_image_access);
    FUNCTION_0("testImageGeneration", test_image_generation);
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

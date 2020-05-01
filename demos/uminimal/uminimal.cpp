
#include "sciter-x.h"
#include "sciter-x-graphics.hpp"
#include "sciter-x-window.hpp"

#include <functional>

// native API demo

// native functions exposed to script
//static int native_sum(int a, int b) { return a + b; }
//static int native_sub(int a, int b) { return a - b; }


static sciter::value native_api() {

  sciter::value api_map;
  sciter::value api_math_map;

  std::function<int(int,int)> native_sum = [](int a, int b) { return a + b; };
  std::function<int(int,int)> native_sub = [](int a, int b) { return a - b; };

  api_math_map.set_item(sciter::value("sum"), sciter::vfunc( native_sum ));
  api_math_map.set_item(sciter::value("sub"), sciter::vfunc( native_sub ));

  api_map.set_item(sciter::value("math"), api_math_map);

  /*
    return {
      math: {
        sum: {native_sum},
        sub: {native_sub},
      }
    }
  */
  return api_map;
}

static sciter::value test_image_access(sciter::value vimg)
{
  sciter::image img = sciter::image::from(vimg); /// failed in this code
  UINT w, h;
  img.dimensions(w, h);
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
    FUNCTION_0("helloWorld", helloWorld);
    FUNCTION_0("nativeApi", native_api);
  END_FUNCTION_MAP

  sciter::string  helloWorld() { return WSTR("Hello u-minimal World"); }

};

#include "resources.cpp"

int uimain(std::function<int()> run ) {

  sciter::archive::instance().open(aux::elements_of(resources)); // bind resources[] (defined in "resources.cpp") with the archive

  frame *pwin = new frame();

  // note: this:://app URL is dedicated to the sciter::archive content associated with the application
  pwin->load( WSTR("this://app/main.htm") );

  pwin->expand();

  return run();

}

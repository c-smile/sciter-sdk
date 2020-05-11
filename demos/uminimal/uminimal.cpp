
#include "sciter-x.h"
#include "sciter-x-graphics.hpp"
#include "sciter-x-window.hpp"
#include "sciter-om.h"

#include <functional>

// native API demo

// native functions exposed to script
//static int NativeMath.sum(int a, int b) { return a + b; }
//static int NativeMath.sub(int a, int b) { return a - b; }

struct NativeMath : public sciter::om::asset<NativeMath> {

  int sum(int a, int b) { return a + b; };
  int sub(int a, int b) { return a - b; };

  SOM_PASSPORT_BEGIN(NativeMath)
    SOM_FUNCS( SOM_FUNC(sum), SOM_FUNC(sub))
  SOM_PASSPORT_END

};


/*
static sciter::value test_image_access(sciter::value vimg)
{
  sciter::image img = sciter::image::from(vimg); /// failed in this code
  UINT w, h;
  img.dimensions(w, h);
  sciter::bytes_writer bw;
  img.save(bw, SCITER_IMAGE_ENCODING_RAW);
  return sciter::value();
}*/


class frame: public sciter::window {
public:
  frame() : window(SW_TITLEBAR | SW_RESIZEABLE | SW_CONTROLS | SW_MAIN | SW_ENABLE_DEBUG) {}

  SOM_PASSPORT_BEGIN(frame)

    SOM_FUNCS(SOM_FUNC(helloWorld),
              SOM_FUNC(generatedImage))
  
  SOM_PASSPORT_END

  // function for script: view.frame.helloWorld()
  sciter::string  helloWorld() { return WSTR("Hello u-minimal World"); }
  // function for script: view.frame.generatedImage()
  sciter::value   generatedImage()
  {
    BYTE pixmap[16 * 16 * 4];
    memset(pixmap, 0, sizeof(pixmap));
    // image data
    for (int i = 0; i < 16 * 16 * 4; i += 4)
    {
      pixmap[i] = i / 4;
      pixmap[i + 1] = 255 - i / 4;
      pixmap[i + 2] = 255;
      pixmap[i + 3] = 255;
    }
    sciter::image img = sciter::image::create(16, 16, true, pixmap);
    return img.to_value();
  }



};

#include "resources.cpp"

int uimain(std::function<int()> run ) {

  // adding NativeMath as a global namespace object,
  // its member functions are accessible as NativeMath.sum() and NativeMath.sub() 
  SciterSetGlobalAsset(new NativeMath());

  sciter::archive::instance().open(aux::elements_of(resources)); // bind resources[] (defined in "resources.cpp") with the archive

  frame *pwin = new frame();

  // note: this:://app URL is dedicated to the sciter::archive content associated with the application
  pwin->load( WSTR("this://app/main.htm") );

  pwin->expand();

  return run();

}

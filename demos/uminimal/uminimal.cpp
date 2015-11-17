
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

  //pwin->load( WSTR("file:///home/andrew/Desktop/test.htm") );


  pwin->expand();

  return run();

}

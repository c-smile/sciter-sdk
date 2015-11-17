#include "sciter-x-window.hpp"

class frame: public sciter::window {
public:
  frame() : window( SW_MAIN | SW_TITLEBAR | SW_RESIZEABLE | SW_CONTROLS ) {}
};

#include "resources.cpp"

int uimain(std::function<int()> run ) {

#ifdef _DEBUG
  sciter::debug_output_console console;
#endif

  sciter::archive::instance().open(aux::elements_of(resources)); // bind resources[] (defined in "resources.cpp") with the archive

  frame *pwin = new frame();

  // note: this:://app URL is dedicated to the sciter::archive content associated with the application
  pwin->load( WSTR("this://app/facade.htm") );

  pwin->expand();

  return run();

}

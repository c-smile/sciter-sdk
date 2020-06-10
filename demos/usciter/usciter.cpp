
#include "sciter-x-window.hpp"
#include "sciter-x-graphics.hpp"

#include "../sqlite/sciter-sqlite.h"



class uSciter: public sciter::window {
public:
  uSciter() : window(SW_TITLEBAR | SW_RESIZEABLE | SW_CONTROLS | SW_MAIN | SW_GLASSY | SW_ENABLE_DEBUG) {}

  //int foo(int p) { return p + 42; }
  //int bar(int p1, int p2) { return p1 + p2; }
  //std::vector<int> vector() { return {1,2,3}; }

  int get_windowHandle() {
    return (int)(intptr_t)get_hwnd();
  }

  SOM_PASSPORT_BEGIN(uSciter)
    //SOM_FUNCS(
      //SOM_FUNC(foo),
      //SOM_FUNC(bar),
      //SOM_FUNC(vector),
    //)
    SOM_PROPS(SOM_RO_VIRTUAL_PROP(windowHandle,get_windowHandle))
  SOM_PASSPORT_END


  /* sample of handling DOM events, click on <button#foo> here:
  virtual bool handle_event(HELEMENT, BEHAVIOR_EVENT_PARAMS& params) {
    sciter::dom::element target = params.heTarget;
    switch (params.cmd) {
      case BUTTON_CLICK:
        if (target.test("button#foo")) {
          target.set_html((const unsigned char*)"foo", 3);
          return true;
        }
    }
    return false;
  }*/

};

#include "resources.cpp"

int uimain(std::function<int()> run ) {

  // enable features to be used from script
  SciterSetOption(NULL, SCITER_SET_SCRIPT_RUNTIME_FEATURES,
                          ALLOW_FILE_IO |
                          ALLOW_SOCKET_IO |
                          ALLOW_EVAL |
                          ALLOW_SYSINFO );

  SciterSetGlobalAsset(new sqlite::SQLite()); // adding SQLite as a global namespace object

#ifdef _DEBUG
//  sciter::debug_output_console console; //- uncomment it if you will need console window
#endif

  sciter::archive::instance().open(aux::elements_of(resources)); // bind resources[] (defined in "resources.cpp") with the archive

  sciter::om::hasset<uSciter> pwin = new uSciter();

  // example, setting "usciter" media variable, check https://sciter.com/forums/topic/debugging-issues/
  SciterSetMediaType(pwin->get_hwnd(), WSTR("desktop,usciter"));

  bool loaded = false;

  const std::vector<sciter::string>& argv = sciter::application::argv();

  // usciter.exe -o file-to-open.htm
  if (argv.size() > 1) {
    sciter::string file_to_open;
    for(int n = 1; n < int(argv.size()) - 1; ++n )
        if( aux::chars_of(argv[n]) == const_wchars("-o") ) {
          file_to_open = argv[n+1];
          break;
        }
    if (file_to_open.length())
      loaded = pwin->load(file_to_open.c_str());
  }

  if(!loaded)
  // note: this:://app URL is dedicated to the sciter::archive content associated with the application
#if defined(WINDOWS)
    pwin->load(WSTR("this://app/default-win.htm"));
#elif defined(OSX)
    pwin->load(WSTR("this://app/default-osx.htm"));
#else // too many of them differnt
    pwin->load(WSTR("this://app/default-else.htm"));
#endif // WINDOWS

  pwin->expand();

  return run();

}

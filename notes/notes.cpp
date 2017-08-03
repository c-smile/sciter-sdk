
#include "sciter-x-window.hpp"

#include <functional>

// native API 

#if 0
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
#endif


class frame: public sciter::window 
{
  UINT hk_ticks = 0; // hot key ticks used to detect CTRL+C->C sequence
public:

  frame(bool needs_debugger) : window(SW_TITLEBAR | SW_RESIZEABLE | SW_CONTROLS | SW_MAIN | (needs_debugger ? SW_ENABLE_DEBUG:0)) 
  {
    //register_hotkey();
  }

  BEGIN_FUNCTION_MAP
    FUNCTION_0("argv", get_argv);
  END_FUNCTION_MAP

  sciter::value  get_argv() { 
    std::vector<sciter::value> vargs;
    for (auto& arg : sciter::application::argv())
      vargs.push_back(sciter::value(arg));
    return sciter::value::make_array(vargs.size(), &vargs[0]);
  }

  void on_new_note_hotkey() {
    this->call_function("handleNewNoteHotkey");
  }

#if 0

  CTRL+C+C does not work reliably, need investigation 

  void register_hotkey() {
#if defined(WINDOWS)
    RegisterHotKey(get_hwnd(), 100, MOD_CONTROL, 'C');
#endif // WINDOWS
  }

#if defined(WINDOWS)
  virtual LRESULT on_message(HWINDOW hwnd, UINT msg, WPARAM wParam, LPARAM lParam, BOOL& handled) override 
  {
    switch(msg) {
      case WM_HOTKEY:
        if (wParam == 100 && GetActiveWindow() != get_hwnd()) { // CTRL+C in other window 
          UINT ticks = GetTickCount();
          UINT delta = ticks - hk_ticks;
          this->hk_ticks = ticks;
          if(delta < 400) 
            on_new_note_hotkey(); // two CTRL+C in the row
        }
        break;
    }
    return 0;
  }
#endif
#endif 

};

#include "resources.cpp"

int uimain(std::function<int()> run ) {

  sciter::archive::instance().open(aux::elements_of(resources)); // bind resources[] (defined in "resources.cpp") with the archive

  auto args = sciter::application::argv();
  bool needs_debugger = std::find(args.cbegin(), args.cend(), WSTR("-debug")) != args.cend();

  frame *pwin = new frame(needs_debugger);

  // note: this:://app URL is dedicated to the sciter::archive content associated with the application
  pwin->load( WSTR("this://app/main.htm") );

  //pwin->load( WSTR("file:///home/andrew/Desktop/test.htm") );


  pwin->expand();

  return run();

}

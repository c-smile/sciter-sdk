
#include "sciter-x-window.hpp"

#include <functional>



class frame: public sciter::window 
{
  UINT hk_ticks = 0; // hot key ticks used to detect CTRL+C->C sequence
public:

  frame(bool needs_debugger) : window(SW_TITLEBAR | SW_RESIZEABLE | SW_CONTROLS | SW_GLASSY | SW_MAIN | (needs_debugger ? SW_ENABLE_DEBUG:0)) 
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
    return sciter::value::make_array(UINT(vargs.size()), &vargs[0]);
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

  SciterSetOption(NULL, SCITER_SET_SCRIPT_RUNTIME_FEATURES,
    ALLOW_FILE_IO |
    ALLOW_SOCKET_IO |
    ALLOW_EVAL |
    ALLOW_SYSINFO);

  sciter::archive::instance().open(aux::elements_of(resources)); // bind resources[] (defined in "resources.cpp") with the archive

  auto args = sciter::application::argv();
  bool needs_debugger = std::find(args.cbegin(), args.cend(), WSTR("-debug")) != args.cend();

  frame *pwin = new frame(needs_debugger);

  // note: this:://app URL is dedicated to the sciter::archive content associated with the application
  //auto t = GetTickCount();
  pwin->load( WSTR("this://app/boot.htm") );
  pwin->expand();
  //t = GetTickCount() - t;
  //SetWindowText()

  
  return run();

}

#pragma once

#include "resource.h"

#include "sciter-x-api.h"
#include "aux-asset.h"
#include "sciter-x-threads.h"
#include "sciter-x-dom.hpp"
#include "sciter-x-host-callback.h"
#include "sciter-x-request.hpp"
#include <deque>

extern HINSTANCE ghInstance;

namespace sciter
{

  class frame
    : public sciter::event_handler
    , public sciter::host<frame>
  {
    HWND   _hwnd;
    frame *_next,*_prev; // linked list of all frames

    static frame *_first;
    static sync::mutex _guard;

    static LRESULT CALLBACK	wnd_proc(HWND, UINT, WPARAM, LPARAM);
    static frame* ptr(HWND hwnd);
    static bool init_class();

    void forget(bool and_release = true);

  public:
    // notification_handler traits:
    HWND      get_hwnd() { return _hwnd; }
    HINSTANCE get_resource_instance() { return ghInstance; }

    //frame(const wchar_t* url);
    virtual ~frame() { forget(false); }

    virtual bool on_close() { return false; }
            
            bool is_valid() const { return _hwnd != 0 && ::IsWindow(_hwnd); }

    virtual void close() /* thread safe*/ { ::PostMessage(_hwnd,WM_CLOSE,0,0); };

    sciter::string get_title();
    void           set_title(const sciter::string& title);

    void           enable_debugger(bool onoff) { SciterSetOption(get_hwnd(),SCITER_SET_DEBUG_MODE,onoff?TRUE:FALSE); }

    sciter::value  debug(unsigned argc, const sciter::value* arg);

    virtual LRESULT on_load_data(LPSCN_LOAD_DATA pnmld)
    {
      LPCBYTE pb = 0; UINT cb = 0;
      aux::wchars wu = aux::chars_of(pnmld->uri);

      // for testing purposes, delayed data delivery
      if (wu.like(WSTR("test:*")))
      {
        sciter::thread(deliver_data_thread, sciter::request(pnmld->requestId));
        return LOAD_MYSELF; // mark request as pending
      }
      return sciter::host<frame>::on_load_data(pnmld);
    }

    // for testing purposes, delayed data delivery
    static void deliver_data_thread(sciter::request rq) {
      // simulate tough job of data downloading
      sciter::sync::sleep(200);
      // let Sciter know that data is delivered. 
      rq.succeeded(200, (LPCBYTE)"hello world", 11);
    }
    
  protected:
    frame();
    bool setup(const wchar_t* url,HWND parent = NULL); // instance
    virtual void init_window() {}
    virtual void init_instance(const dom::element& root_el) {}
    virtual bool on_message(UINT, WPARAM, LPARAM, LRESULT&) { return false; }

    BEGIN_FUNCTION_MAP
      FUNCTION_V("debug", debug);
    END_FUNCTION_MAP

  };

  class main_frame: public frame
  {
    bool   _is_glassy; // true - transparent

  public:

    main_frame(const wchar_t* url = L"res:default.htm");

    //virtual bool on_close() 
    //{ 
    //  ::DestroyWindow(get_hwnd());
    //  ::PostQuitMessage(0);
    //  return true;
    //}

  protected:
    virtual void init_instance(const dom::element& root_el) override; // instance
    virtual void init_window() override;

    virtual bool on_message(UINT, WPARAM, LPARAM, LRESULT&);

    BEGIN_FUNCTION_MAP
      FUNCTION_0("title", get_title);
      FUNCTION_1("title", set_title);
      FUNCTION_V("log", debug);
      FUNCTION_1("open",  open);
      FUNCTION_2("open",  open);
      FUNCTION_0("getNativeFunctorTest", get_native_functor_test);
      FUNCTION_0("inspectorIsPresent", inspector_is_present);
      //FUNCTION_1("testArray",  testArray);
      CHAIN_FUNCTION_MAP(frame);
    END_FUNCTION_MAP

    sciter::value get_title();
    sciter::value set_title(sciter::value title);
    sciter::value open(sciter::value url, sciter::value param = sciter::value());
    sciter::value get_native_functor_test();
    sciter::value inspector_is_present();

    /*sciter::value testArray(const sciter::value& data)
    {
        double vals[4] ={0};
        bool isObjArray = data.is_object_array();
        int length = data.length();
        for(int i1 = 0 ;i1 < length; i1++)
        {
            sciter::value it = data.get_item(i1);
            bool t0 = it.is_object();
            bool t1 = it.is_float();
            bool t2 = it.is_int();
            bool t3 = it.is_array();
            bool t4 = it.is_object_array();
            vals[i1] = data.get_item(i1).get<double>();
        }
        return sciter::value();
     }*/

  };


}
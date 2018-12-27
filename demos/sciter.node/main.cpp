#include "view.h"
#include "napi-common.h"

EXTERN_C ISciterAPI* SCAPI SciterAPI();

static napi_value pumpMessages(napi_env env, napi_callback_info info) {
  napi_value res;

  View::event_env_ = env;

  bool r = sciter::application::pump_messages();

  View::event_env_ = nullptr;

  NAPI_CALL(env, napi_get_boolean(env, r, &res));

  return res;
}

typedef enum _Process_DPI_Awareness {
  Process_DPI_Unaware = 0,
  Process_System_DPI_Aware = 1,
  Process_Per_Monitor_DPI_Aware = 2
} Process_DPI_Awareness;

typedef HRESULT WINAPI SetProcessDPIAwareness_t(
  _In_  Process_DPI_Awareness value
);
typedef HRESULT WINAPI GetProcessDPIAwareness_t(
  _In_   HANDLE hprocess,
  _Out_  Process_DPI_Awareness *value
);

void set_dpi_ready() {
  static SetProcessDPIAwareness_t* _SetProcessDPIAwareness = nullptr;
  if (!_SetProcessDPIAwareness) {
    HMODULE hm = LoadLibrary(L"shcore.dll");
    if (hm)
      _SetProcessDPIAwareness = reinterpret_cast<SetProcessDPIAwareness_t*>(GetProcAddress(hm, "SetProcessDpiAwareness"));
  }
  if (_SetProcessDPIAwareness) {
    HRESULT hr = _SetProcessDPIAwareness(Process_Per_Monitor_DPI_Aware);
    assert(SUCCEEDED(hr));
  }
}


NAPI_MODULE_INIT() {

  set_dpi_ready();     // set high-dpi support
  SAPI(SciterAPI());   // init SciterAPI
  OleInitialize(NULL); // Sciter needs this too

  SciterSetOption(NULL, SCITER_SET_SCRIPT_RUNTIME_FEATURES,
    ALLOW_FILE_IO |
    ALLOW_SOCKET_IO |
    ALLOW_EVAL |
    ALLOW_SYSINFO);

  napi_property_descriptor desc = DECLARE_NAPI_PROPERTY("pumpMessages", pumpMessages);
  NAPI_CALL(env, napi_define_properties(env, exports, 1, &desc));
  View::Init(env, exports);
  return exports;
}

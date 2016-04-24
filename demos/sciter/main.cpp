#include "stdafx.h"
#include <shellapi.h>
#include "sciter.h"
#include "sciter-x-dom.hpp"
#include "sciter-x-debug.h"

HINSTANCE ghInstance = 0;

typedef enum _Process_DPI_Awareness { 
  Process_DPI_Unaware            = 0,
  Process_System_DPI_Aware       = 1,
  Process_Per_Monitor_DPI_Aware  = 2
} Process_DPI_Awareness;

typedef HRESULT WINAPI SetProcessDPIAwareness_t(
  _In_  Process_DPI_Awareness value
);
typedef HRESULT WINAPI GetProcessDPIAwareness_t(
  _In_   HANDLE hprocess,
  _Out_  Process_DPI_Awareness *value
);


void check_dpi_ready() {
  static GetProcessDPIAwareness_t* _GetProcessDPIAwareness = nullptr;
  if(!_GetProcessDPIAwareness) {
    HMODULE hm = LoadLibrary(L"shcore.dll");
    if( hm )
      _GetProcessDPIAwareness = reinterpret_cast<GetProcessDPIAwareness_t*>(GetProcAddress(hm,"GetProcessDpiAwareness"));
  }
  if(_GetProcessDPIAwareness) {
    Process_DPI_Awareness aw; 
    HRESULT hr =_GetProcessDPIAwareness(NULL, &aw);
	assert(SUCCEEDED(hr));
	assert(aw == Process_Per_Monitor_DPI_Aware);
  }
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPTSTR    lpCmdLine,
                      int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);


  OleInitialize(0); // for system drag-n-drop

  check_dpi_ready();

  ghInstance = hInstance;

  SciterClassName();
  sciter::debug_output debug;

  // SciterAppendMasterCSS test, adding <checkbox>text</checkbox> element.
  //const char* def = "checkbox { display:inline-block; display-model:inline-inside; style-set: \"std-checkbox\"; }";
  //SciterAppendMasterCSS((const byte*)def,UINT(strlen(def))); 
    
  std::wstring file_name = L"res:default.htm";
  
  // command line parsing to handle invocation from Windows Explorer.
  int n_args = 0;
  LPWSTR *args = CommandLineToArgvW(GetCommandLineW(), &n_args);
  
  if( args )
  {
      if( n_args >= 2 ) {
        auto last_prm = aux::chars_of(args[n_args - 1]);
        if( last_prm.like(L"*\\*") || last_prm.like(L"*/*") )
        {
          if( last_prm.like(L"\"*\"") )
            file_name = std::wstring( last_prm.start + 1, last_prm.end() - 1);
          else
            file_name = std::wstring( last_prm.start, last_prm.end());
        }
      }
      LocalFree(args);
  }

   	
	MSG msg;
	HACCEL hAccelTable;

  {
    aux::asset_ptr<sciter::main_frame> wnd = new sciter::main_frame(file_name.c_str());

	  if (!wnd || !wnd->is_valid())
    {
		  return FALSE;
    }

	  hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SCITER));

	  // Main message loop:
	  while (wnd->is_valid() && GetMessage(&msg, NULL, 0, 0))
	  {
		  if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		  {
			  TranslateMessage(&msg);
			  DispatchMessage(&msg);
		  }
	  }
  }

  OleUninitialize();

	return (int) msg.wParam;
}

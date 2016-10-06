// plain-win.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "plain-win.h"

HINSTANCE ghInstance = 0;

#define WINDOW_CLASS_NAME L"sciter-plain-window"			// the main window class name


//
//  FUNCTION: window::init()
//
//  PURPOSE: Registers the window class.
//
bool window::init_class()
{
  static ATOM cls = 0;
  if( cls ) 
    return true;

	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= wnd_proc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= ghInstance;
	wcex.hIcon			= LoadIcon(ghInstance, MAKEINTRESOURCE(IDI_PLAINWIN));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0;//MAKEINTRESOURCE(IDC_PLAINWIN);
	wcex.lpszClassName	= WINDOW_CLASS_NAME;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	cls = RegisterClassEx(&wcex);
  return cls != 0;
}

window::window()
{
  init_class();
  _hwnd = CreateWindow(WINDOW_CLASS_NAME, L"demo", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, ghInstance, this);
  if (!_hwnd)
     return;

  init();

  ShowWindow(_hwnd, SW_SHOW);
  UpdateWindow(_hwnd);
}

window* window::ptr(HWND hwnd)
{
  return reinterpret_cast<window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
}

bool window::init()
{
   SetWindowLongPtr(_hwnd, GWLP_USERDATA, LONG_PTR(this));
   setup_callback();
   load_file(L"res:default.htm");
   return true;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//

LRESULT CALLBACK window::wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  if( message == WM_KEYDOWN && wParam == VK_F5 )
  {
    window* self = ptr(hWnd);
    self->load_file(L"res:default.htm");
    return 0;
  }
  else if( message == WM_KEYDOWN && wParam == VK_F2 )
  {
    // testing sciter::host<window>::call_function() and so SciterCall ()
    window* self = ptr(hWnd);
    sciter::dom::element root = self->get_root();

    sciter::value r;
    try {
      r = root.call_function("Test.add",sciter::value(2),sciter::value(2));
    } catch (sciter::script_error& err) {
      std::cerr << err.what() << std::endl;
    }
    //or sciter::value r = self->call_function("Test.add",sciter::value(2),sciter::value(2));
    assert(r.is_int() && r.get(0) == 4);
    return 0;
  }
  else if( message == WM_KEYDOWN && wParam == VK_F9 )
  {
    window* self = ptr(hWnd);
    self->load_file(L"http://terrainformatica.com/tests/test.htm");
    return 0;
  }
  else if( message == WM_KEYDOWN && wParam == VK_F8 )
  {
    window* self = ptr(hWnd);
    sciter::dom::element root = self->get_root();
    sciter::dom::element el_time = root.find_first("input#time");
    ULARGE_INTEGER ft; GetSystemTimeAsFileTime((FILETIME*)&ft);
    el_time.set_value( sciter::value::date(ft.QuadPart) );

    return 0;
  }
  if( message == WM_KEYDOWN && wParam == VK_F1 )
  {
    window* self = ptr(hWnd);
    sciter::dom::element root = self->get_root();
    sciter::dom::element first = root.find_first("#first");
    sciter::dom::element second = root.find_first("#second");

    auto fw = first.get_style_attribute("width");
    auto sw = second.get_style_attribute("width");

    return 0;
  }



  //date_time::DT_UTC | date_time::DT_HAS_DATE | date_time::DT_HAS_TIME | date_time::DT_HAS_SECONDS


//SCITER integration starts
  BOOL handled = FALSE;
  LRESULT lr = SciterProcND(hWnd,message,wParam,lParam, &handled);
  if( handled )
    return lr;
//SCITER integration ends
  
  window* self = ptr(hWnd);
  
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

// the WinMain

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

  ghInstance = hInstance;

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

  // Perform application initialization:

  OleInitialize(NULL); // for shell interaction: drag-n-drop, etc.

  //SciterClassName();

  //return 0;

  window wnd;

	if (!wnd.is_valid())
		return FALSE;

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PLAINWIN));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

  OleUninitialize();

	return (int) msg.wParam;
}

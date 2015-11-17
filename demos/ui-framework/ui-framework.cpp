// ui-framework.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ui-framework.h"

HINSTANCE ghInstance = 0;

#define WINDOW_CLASS_NAME L"sciter-ui-frame"			// the main window class name

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
	wcex.hIcon			= LoadIcon(ghInstance, MAKEINTRESOURCE(IDI_UIFRAMEWORK));
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
   setup_callback(); // callbacks
   sciter::attach_dom_event_handler(_hwnd,this); // dom events and callbacks

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

  sciter::sync::gui_thread_ctx _; // instance of gui_thread_ctx
                                  // it should be created as a variable inside WinMain 
                                  // gui_thread_ctx is critical for GUI_CODE_START/END to work
  
  OleInitialize(NULL); // for shell interaction: drag-n-drop, etc.

  window wnd;

	if (!wnd.is_valid())
		return FALSE;

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_UIFRAMEWORK));

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





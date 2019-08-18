// sciter-opengl.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "sciter-opengl.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name


HWND  hWnd;
HGLRC hglrc_main = NULL;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

sciter::dom::element back_layer;
sciter::dom::element fore_layer;

void init_3d();
void stop_3d();
void draw_3d();


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	// Sciter needs it for Drag-n-drop, etc.
	OleInitialize(NULL);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SCITEROPENGL, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }
	    
	context* ctx = nullptr;

    MSG msg;

	bool quit = false;
	bool first = false;

    // Main message loop:
    while (!quit) 
	  {
		  if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		  {
			  if(msg.message == WM_QUIT)
			  {
				  quit = true;
			  }
			  else {
				  TranslateMessage(&msg);
				  DispatchMessage(&msg);
			  }
		  }
		  else {

			  if( !hglrc_main) {
          hglrc_main = wglGetCurrentContext();
          init_3d();
			  }

        /*{
          // draw background layer if needed
          SCITER_X_MSG_PAINT pc(back_layer, FALSE);
          SciterProcX(hWnd, pc);
        }*/

        draw_3d();

			  {
				  // draw foreground layer on top of 3d
				  SCITER_X_MSG_PAINT pc(fore_layer, TRUE);
				  SciterProcX(hWnd, pc);
			  }
        glFinish();
			  {
				  HDC dc = GetDC(hWnd);
				  SwapBuffers(dc);
				  ReleaseDC(hWnd, dc);
			  }

		  }
	  }
    stop_3d();

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SCITEROPENGL));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SCITEROPENGL);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

UINT SC_CALLBACK SciterCallback(LPSCITER_CALLBACK_NOTIFICATION pns, LPVOID callbackParam);

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			return 0;
		}
	}

	BOOL handled = FALSE;
	LRESULT lr;
	lr = SciterProcND(hWnd, message, wParam, lParam, &handled);
	if (handled)
		return lr;

    switch (message)
    {
	  case WM_CREATE:
		{
			SciterSetOption(hWnd, SCITER_SET_DEBUG_MODE, TRUE);
			// 2. setup callback (resource loading, etc):
			SciterSetCallback(hWnd, SciterCallback, NULL);
			// 2b. setup DOM event handler:
			//sciter::attach_dom_event_handler(hWnd, &g_ViewDomEventHandler);
			// 3. load HTML content in it:
			SciterLoadFile(hWnd, L"res:facade.htm");

			// 4. get layer elements:
			sciter::dom::element root = sciter::dom::element::root_element(hWnd);
			back_layer = root.find_first("section#back-layer");
			fore_layer = root.find_first("section#fore-layer");
			assert(back_layer && fore_layer);
		}
		break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


// handle SC_LOAD_DATA requests - get data from resources of this application
UINT DoLoadData(LPSCN_LOAD_DATA pnmld)
{
	LPCBYTE pb = 0; UINT cb = 0;
	aux::wchars wu = aux::chars_of(pnmld->uri);
	if (wu.like(WSTR("res:*")))
	{
		// then by calling possibly overloaded load_resource_data method
		if (sciter::load_resource_data(hInst, wu.start + 4, pb, cb))
			::SciterDataReady(pnmld->hwnd, pnmld->uri, pb, cb);
	}
	else if (wu.like(WSTR("this://app/*"))) {
		// try to get them from archive (if any, you need to call sciter::archive::open() first)
		aux::bytes adata = sciter::archive::instance().get(wu.start + 11);
		if (adata.length)
			::SciterDataReady(pnmld->hwnd, pnmld->uri, adata.start, adata.length);
	}
	return LOAD_OK;
}

// fulfill SC_ATTACH_BEHAVIOR request 
UINT DoAttachBehavior(LPSCN_ATTACH_BEHAVIOR lpab)
{
	sciter::event_handler *pb = sciter::behavior_factory::create(lpab->behaviorName, lpab->element);
	if (pb)
	{
		lpab->elementTag = pb;
		lpab->elementProc = sciter::event_handler::element_proc;
		return TRUE;
	}
	return FALSE;
}

UINT SC_CALLBACK SciterCallback(LPSCITER_CALLBACK_NOTIFICATION pns, LPVOID callbackParam)
{
	// here are all notifiactions
	switch (pns->code)
	{
	case SC_LOAD_DATA:          return DoLoadData((LPSCN_LOAD_DATA)pns);
	case SC_ATTACH_BEHAVIOR:    return DoAttachBehavior((LPSCN_ATTACH_BEHAVIOR)pns);
	}
	return 0;
}
// SCITER -


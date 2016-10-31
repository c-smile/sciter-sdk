// minimal.cpp : minimal Sciter application.

#include <windows.h>

#include "sciter-x.h"

HINSTANCE ghInstance = 0;

// generate BGRA image
// check http://sciter.com/forums/topic/how-to-render-memory-image-in-sciter-page/
void GenerateBGRATestsImage(HWND hSciter) {
  BYTE packedData[4 + 4 + 4 + 10 * 10 * 4]; 
  memset(packedData,0,sizeof(packedData));

  // signature
  packedData[0] = 'B';
	packedData[1] = 'G';
	packedData[2] = 'R';
  packedData[3] = 'A';

  // width/height
  packedData[7] = 10;
  packedData[11] = 10;

  // image data
  for ( int i = 0; i < 10 * 10 * 4; i += 4)
  {
			packedData[4 + 4 + 4 + i] = i / 4;
      packedData[4 + 4 + 4 + i + 1] = 255 - i / 4;
      packedData[4 + 4 + 4 + i + 2] = 255;
      packedData[4 + 4 + 4 + i + 3] = 255;
  }
  ::SciterDataReady( hSciter, WSTR("in-memory:test"), packedData,  sizeof(packedData));
}

// handle SC_LOAD_DATA requests - get data from resources of this application
UINT DoLoadData(LPSCN_LOAD_DATA pnmld)
{
  LPCBYTE pb = 0; UINT cb = 0;
  aux::wchars wu = aux::chars_of(pnmld->uri);

  if(wu == const_wchars("in-memory:test"))
  {
    GenerateBGRATestsImage(pnmld->hwnd);
  }
  else if(wu.like(WSTR("res:*")))
  {
    // then by calling possibly overloaded load_resource_data method
    if(sciter::load_resource_data(ghInstance,wu.start+4, pb, cb))
      ::SciterDataReady( pnmld->hwnd, pnmld->uri, pb,  cb);
  } else if(wu.like(WSTR("this://app/*"))) {
    // try to get them from archive (if any, you need to call sciter::archive::open() first)
    aux::bytes adata = sciter::archive::instance().get(wu.start+11);
    if(adata.length)
      ::SciterDataReady( pnmld->hwnd, pnmld->uri, adata.start, adata.length);
  }
  return LOAD_OK;
}

// fulfill SC_ATTACH_BEHAVIOR request 
UINT DoAttachBehavior( LPSCN_ATTACH_BEHAVIOR lpab )
{
  sciter::event_handler *pb = sciter::behavior_factory::create(lpab->behaviorName, lpab->element);
  if(pb)
  {
    lpab->elementTag  = pb;
    lpab->elementProc = sciter::event_handler::element_proc;
    return TRUE;
  }
  return FALSE;
}

UINT SC_CALLBACK SciterCallback( LPSCITER_CALLBACK_NOTIFICATION pns, LPVOID callbackParam )
{
  // here are all notifiactions
  switch(pns->code) 
  {
    case SC_LOAD_DATA:          return DoLoadData((LPSCN_LOAD_DATA) pns);
    case SC_ATTACH_BEHAVIOR:    return DoAttachBehavior((LPSCN_ATTACH_BEHAVIOR)pns );
  }
  return 0;
}


LPCWSTR GetUrl() {
  static WCHAR url[MAX_PATH] = {0};

  wcscpy(url,L"file://");
  GetModuleFileName(NULL,url+7,MAX_PATH-7);
  WCHAR* t = wcsrchr(url,'\\');
  assert(t);
  wcscpy(t + 1,L"minimal.htm");
  return url;
}


int CALLBACK WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
  ghInstance = hInstance;
  OleInitialize(NULL); // for shell interaction: drag-n-drop, etc.

  // un-comment the following to see console output: 
  //sciter::debug_output_console _;

  // un-comment the following to enable inspector in this application 
  // SciterSetOption(NULL, SCITER_SET_DEBUG_MODE, TRUE);

	HWND wnd = ::CreateWindowEx(
		0, /*WS_EX_LAYOUTRTL,*/
		::SciterClassName(),
		L"Minimal Sciter Application",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0,
		800, 600,
		0,0,0,0);

  ::SciterSetCallback(wnd,&SciterCallback,NULL);
    
	::SciterLoadFile(wnd, GetUrl());
	::ShowWindow(wnd, SW_SHOWNORMAL);

/*  sciter::value undefined, map = sciter::value::from_string(aux::chars_of(L"{'0': 0, '1': 1, '2': 2, '3': 3}"), CVT_JSON_LITERAL);
  map.set_item(L"1", undefined);
  map.set_item(L"3", undefined);
  sciter::string s = map.to_string(CVT_JSON_LITERAL);
  s = s; */
	MSG msg;
  while( ::IsWindow(wnd) && GetMessage(&msg, 0, 0, 0) )
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

  return 0;
}
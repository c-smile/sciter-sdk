// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "AxSciter_i.h"
#include "dllmain.h"
#include "xdlldata.h"

CAxSciterModule _AtlModule;

HINSTANCE ghInstance = 0;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
#ifdef _MERGE_PROXYSTUB
	if (!PrxDllMain(hInstance, dwReason, lpReserved))
		return FALSE;
#endif
  ghInstance = hInstance;
	return _AtlModule.DllMain(dwReason, lpReserved); 
}

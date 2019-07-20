//--------------------------------------------------------------------------------------
//
// This application demonstrates Sciter/DirectX integration.
//
// Sciter additions: Terra Informatica Software, Inc.
// marked by SCITER+/SCITER- brackets.
//--------------------------------------------------------------------------------------

#include "stdafx.h"

#include <windows.h>
#include <d3d11.h>
//#include <d3dx11.h>
#include "resource.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

// SCITER+
#include "sciter-x.h"
// SCITER-

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
HINSTANCE                           g_hInst = NULL;
HWND                                g_hWnd = NULL;
D3D_DRIVER_TYPE                     g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL                   g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*                       g_pd3dDevice = NULL;
ID3D11DeviceContext*                g_pImmediateContext = NULL;
IDXGISwapChain*                     g_pSwapChain = NULL;
ID3D11RenderTargetView*             g_pRenderTargetView = NULL;
//ID3D11Texture2D*                    g_pDepthStencil = NULL;
//ID3D11DepthStencilView*             g_pDepthStencilView = NULL;
ID3D11Buffer*                       g_pCBNeverChanges = NULL;
ID3D11Buffer*                       g_pCBChangeOnResize = NULL;
ID3D11Buffer*                       g_pCBChangesEveryFrame = NULL;
ID3D11ShaderResourceView*           g_pTextureRV = NULL;
ID3D11SamplerState*                 g_pSamplerLinear = NULL;

//SCITER+
                                                 //SCITER-

                                                 //--------------------------------------------------------------------------------------
                                                 // Forward declarations
                                                 //--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
HRESULT InitDevice();
void CleanupDevice();
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void Render();


//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  //SCITER+
  // Sciter needs it for Drag-n-drop, etc.
  OleInitialize(NULL);

  // enable "unsafe" functions to be accessible from script
  SciterSetOption(NULL, SCITER_SET_SCRIPT_RUNTIME_FEATURES,
    ALLOW_FILE_IO |
    ALLOW_SOCKET_IO |
    ALLOW_EVAL |
    ALLOW_SYSINFO);

  //SCITER-

  if (FAILED(InitWindow(hInstance, nCmdShow)))
    return 0;

  if (FAILED(InitDevice()))
  {
    CleanupDevice();
    return 0;
  }

  // Main message loop
  MSG msg = { 0 };
  while (WM_QUIT != msg.message)
  {
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    } else 
      Render();
    
    
  }

  CleanupDevice();

  return (int)msg.wParam;
}


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{
  // Register class
  WNDCLASSEX wcex;
  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_SCITERDX);
  wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = NULL;
  wcex.lpszClassName = L"SciterDxClass";
  wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SCITERDX);
  if (!RegisterClassEx(&wcex))
    return E_FAIL;

  // Create window
  g_hInst = hInstance;
  RECT rc = { 0, 0, 640, 480 };
  AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
  g_hWnd = CreateWindow(L"SciterDxClass", L"Sciter Direct3D 11 Window", WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
    NULL);
  if (!g_hWnd)
    return E_FAIL;

  ShowWindow(g_hWnd, nCmdShow);

  return S_OK;
}


// SCITER +
BOOL InitSciterEngineInstance(HWND hwnd, IDXGISwapChain* pSwapChain);
// SCITER -

//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT InitDevice()
{
  HRESULT hr = S_OK;

  RECT rc;
  GetClientRect(g_hWnd, &rc);
  UINT width = rc.right - rc.left;
  UINT height = rc.bottom - rc.top;

  //SCITER +
  // createDeviceFlags must contain D3D10_CREATE_DEVICE_BGRA_SUPPORT flag
  UINT createDeviceFlags = D3D10_CREATE_DEVICE_BGRA_SUPPORT;
  //SCITER -
#ifdef _DEBUG
  createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  D3D_DRIVER_TYPE driverTypes[] =
  {
    D3D_DRIVER_TYPE_HARDWARE,
    D3D_DRIVER_TYPE_WARP,
    D3D_DRIVER_TYPE_REFERENCE,
  };
  UINT numDriverTypes = ARRAYSIZE(driverTypes);

  D3D_FEATURE_LEVEL featureLevels[] =
  {
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_10_1,
    D3D_FEATURE_LEVEL_10_0,
  };
  UINT numFeatureLevels = ARRAYSIZE(featureLevels);

  DXGI_SWAP_CHAIN_DESC sd;
  ZeroMemory(&sd, sizeof(sd));
  sd.BufferCount = 1;
  sd.BufferDesc.Width = width;
  sd.BufferDesc.Height = height;
  sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  sd.BufferDesc.RefreshRate.Numerator = 60;
  sd.BufferDesc.RefreshRate.Denominator = 1;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow = g_hWnd;
  sd.SampleDesc.Count = 1;
  sd.SampleDesc.Quality = 0;
  sd.Windowed = TRUE;

  for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
  {
    g_driverType = driverTypes[driverTypeIndex];
    hr = D3D11CreateDeviceAndSwapChain(NULL, g_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
      D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext);
    if (SUCCEEDED(hr))
      break;
  }
  if (FAILED(hr))
    return hr;

  // Create a render target view
  ID3D11Texture2D* pBackBuffer = NULL;
  hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
  if (FAILED(hr))
    return hr;

  hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
  pBackBuffer->Release();
  if (FAILED(hr))
    return hr;

  // Setup the viewport
  D3D11_VIEWPORT vp;
  vp.Width = (FLOAT)width;
  vp.Height = (FLOAT)height;
  vp.MinDepth = 0.0f;
  vp.MaxDepth = 1.0f;
  vp.TopLeftX = 0;
  vp.TopLeftY = 0;
  g_pImmediateContext->RSSetViewports(1, &vp);

  // SCITER+
  // create Sciter engine instance for the window:
  InitSciterEngineInstance(g_hWnd, g_pSwapChain);
  // SCITER-

  // Create the sample state
  D3D11_SAMPLER_DESC sampDesc;
  ZeroMemory(&sampDesc, sizeof(sampDesc));
  sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
  sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
  sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
  sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
  sampDesc.MinLOD = 0;
  sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
  hr = g_pd3dDevice->CreateSamplerState(&sampDesc, &g_pSamplerLinear);
  if (FAILED(hr))
    return hr;

  return S_OK;
}


//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
  if (g_pImmediateContext) g_pImmediateContext->ClearState();

  if (g_pSamplerLinear) g_pSamplerLinear->Release();
  if (g_pTextureRV) g_pTextureRV->Release();
  if (g_pCBNeverChanges) g_pCBNeverChanges->Release();
  if (g_pCBChangeOnResize) g_pCBChangeOnResize->Release();
  if (g_pCBChangesEveryFrame) g_pCBChangesEveryFrame->Release();
  if (g_pRenderTargetView) g_pRenderTargetView->Release();
  if (g_pSwapChain) g_pSwapChain->Release();
  if (g_pImmediateContext) g_pImmediateContext->Release();
  if (g_pd3dDevice) g_pd3dDevice->Release();
}


void Resize()
{
  if (!g_pSwapChain)
    return;

  RECT rc;
  GetClientRect(g_hWnd, &rc);

  int width = rc.right - rc.left;
  int height = rc.top - rc.bottom;

  g_pImmediateContext->OMSetRenderTargets(0, 0, 0);

  // Release all outstanding references to the swap chain's buffers.
  g_pRenderTargetView->Release();

  HRESULT hr;
  // Preserve the existing buffer count and format.
  // Automatically choose the width and height to match the client rect for HWNDs.
  hr = g_pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

  // Perform error handling here!

  // Get buffer and create a render-target-view.
  ID3D11Texture2D* pBuffer;
  hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
    (void**)&pBuffer);
  // Perform error handling here!

  hr = g_pd3dDevice->CreateRenderTargetView(pBuffer, NULL,
    &g_pRenderTargetView);
  // Perform error handling here!
  pBuffer->Release();

  g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, NULL);

  // Set up the viewport.
  D3D11_VIEWPORT vp;
  vp.Width = float(width);
  vp.Height = float(height);
  vp.MinDepth = 0.0f;
  vp.MaxDepth = 1.0f;
  vp.TopLeftX = 0;
  vp.TopLeftY = 0;
  g_pImmediateContext->RSSetViewports(1, &vp);

}

//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  //SCITER +
  // mix-in Sciter functionality 
  if (message != WM_CREATE && message != WM_PAINT) // we create the engine instance later,
                            // by calling SciterCreateOnDirectXWindow(hWnd, pSwapChain);
  {
    BOOL handled = FALSE;
    LRESULT lr = SciterProcND(hWnd, message, wParam, lParam, &handled);
    if (handled)
      return lr;
  }
  //SCITER -

  PAINTSTRUCT ps;
  HDC hdc;

  switch (message)
  {
  case WM_PAINT:
    hdc = BeginPaint(hWnd, &ps);
    EndPaint(hWnd, &ps);
    break;

  case WM_SIZE:
    Resize();
    break;

  case WM_DESTROY:
    PostQuitMessage(0);
    break;

  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }

  return 0;
}


#define MAXSAMPLES 100
static int tickindex = 0;
static int ticksum = 0;
static int ticklist[MAXSAMPLES];

/* need to zero out the ticklist array before starting */
/* average will ramp up until the buffer is full */
/* returns average ticks per frame over the MAXSAMPLES last frames */

double CalcAverageTick(int newtick)
{
  ticksum -= ticklist[tickindex];  /* subtract value falling off */
  ticksum += newtick;              /* add new value */
  ticklist[tickindex] = newtick;   /* save new value so it can be subtracted later */
  tickindex = (tickindex + 1) % MAXSAMPLES;    /* inc buffer index */
    
  /* return average */
  return((double)ticksum / MAXSAMPLES);
}

//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------

int gFPS = 1000;

void Render()
{
  // Update our time
  static float t = 0.0f;
  static int counter = 0;
  if (g_driverType == D3D_DRIVER_TYPE_REFERENCE)
  {
  }
  else
  {
    static DWORD dwTimeStart = 0;
    DWORD dwTimeCur = GetTickCount();
    if (dwTimeStart == 0)
      dwTimeStart = dwTimeCur;
    //float current = 1000.0 / max(1,(dwTimeCur - dwTimeStart));

    double at = CalcAverageTick(dwTimeCur - dwTimeStart);
    dwTimeStart = dwTimeCur;

    gFPS = int(1000.0 / max(1, at));

    if (++counter % 100 == 0) 
    {
      WCHAR buffer[1024];
      wsprintf(buffer, L"FPS %d", gFPS);
      SetWindowText(g_hWnd, buffer);
    }
  }

  //
  // Clear the back buffer
  //
  float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red, green, blue, alpha
  g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, ClearColor);

  // SCITER +
  // render HTML document
  SciterRenderOnDirectXWindow(g_hWnd, NULL, FALSE);
  // SCITER -

  //
  // Present our back buffer to our front buffer
  //
  g_pSwapChain->Present(0, 0);
}

// SCITER +

// view_dom_event_handler, provides native methods to be used in script

struct view_dom_event_handler : public sciter::event_handler 
{
  BEGIN_FUNCTION_MAP
    FUNCTION_0("FPS", FPS)
  END_FUNCTION_MAP

  sciter::value FPS()
  {
    return sciter::value(gFPS);
  }
};

view_dom_event_handler g_ViewDomEventHandler;

UINT SC_CALLBACK SciterCallback(LPSCITER_CALLBACK_NOTIFICATION pns, LPVOID callbackParam);

// create Sciter engine instance for the window:
BOOL InitSciterEngineInstance(HWND hWnd, IDXGISwapChain *pSwapChain)
{
  // 1. create engine instance on the window with the swap chain:
  BOOL r = SciterCreateOnDirectXWindow(hWnd, pSwapChain);
  if (!r) return FALSE;

  SciterSetOption(hWnd, SCITER_SET_DEBUG_MODE, TRUE);

  // 2. setup callback (resource loading, etc):
  SciterSetCallback(hWnd, SciterCallback, NULL);

  // 2b. setup DOM event handler:
  sciter::attach_dom_event_handler(hWnd, &g_ViewDomEventHandler);

  // 3. load HTML content in it:
  r = SciterLoadFile(hWnd, L"res:default.htm");
  assert(r);

  // done
  return true;
}

// handle SC_LOAD_DATA requests - get data from resources of this application
UINT DoLoadData(LPSCN_LOAD_DATA pnmld)
{
  LPCBYTE pb = 0; UINT cb = 0;
  aux::wchars wu = aux::chars_of(pnmld->uri);
  if (wu.like(WSTR("res:*")))
  {
    // then by calling possibly overloaded load_resource_data method
    if (sciter::load_resource_data(g_hInst, wu.start + 4, pb, cb))
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


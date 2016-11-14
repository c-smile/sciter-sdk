//--------------------------------------------------------------------------------------
// File: dxsdk\Samples\C++\Direct3D11\Tutorials\Tutorial07\Tutorial07.cpp
//
// This application demonstrates Sciter/DirectX integration.
//
// Iriginal Tutorial07 Copyright (c) Microsoft Corporation. All rights reserved.
// Sciter additions: Terra Informatica Software, Inc.
// marked by SCITER+/SCITER- brackets.
//--------------------------------------------------------------------------------------
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <xnamath.h>
#include <wrl.h>
#include "resource.h"

#include "include/GL/glew.h"
#include "include/GL/wglew.h"

// SCITER+
#include "sciter-x.h"
// SCITER-

//namespace SciterInterop
//{

#define MY_IID_PPV_ARGS IID_PPV_ARGS
using Microsoft::WRL::ComPtr;

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT2 Tex;
};

struct CBNeverChanges
{
	XMMATRIX mView;
};

struct CBChangeOnResize
{
	XMMATRIX mProjection;
};

struct CBChangesEveryFrame
{
	XMMATRIX mWorld;
	XMFLOAT4 vMeshColor;
};


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

FLOAT                               g_rotationSpeed = 1.0;
FLOAT                               g_colorSpeed = 1.0;

//SCITER+
sciter::dom::element                g_backLayer; // layer
sciter::dom::element                g_foreLayer; // elements
//SCITER-

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow );
HRESULT InitDevice( );
void CleanupDevice( );
LRESULT CALLBACK    WndProc( HWND, UINT, WPARAM, LPARAM );
void Render( );


//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );

	//SCITER+
		// Sciter needs it for Drag-n-drop, etc.
	OleInitialize( NULL );
	//SCITER-

	if (FAILED( InitWindow( hInstance, nCmdShow ) ))
		return 0;

	if (FAILED( InitDevice( ) ))
	{
		CleanupDevice( );
		return 0;
	}

	// Main message loop
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ))
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			Render( );
		}
	}

	CleanupDevice( );

	return (int)msg.wParam;
}


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow )
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof( WNDCLASSEX );
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon( hInstance, (LPCTSTR)IDI_SCITER_DIRECTX );
	wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"TutorialWindowClass";
	wcex.hIconSm = LoadIcon( wcex.hInstance, (LPCTSTR)IDI_SCITER_DIRECTX );
	if (!RegisterClassEx( &wcex ))
		return E_FAIL;

	// Create window
	g_hInst = hInstance;
	RECT rc = { 0, 0, 1024, 768 };
	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
	g_hWnd = CreateWindow( L"TutorialWindowClass", L"Sciter with Direct3D 11 Tutorial 7", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
		NULL );
	if (!g_hWnd)
		return E_FAIL;

	ShowWindow( g_hWnd, nCmdShow );

	return S_OK;
}

// SCITER +
BOOL InitSciterEngineInstance( HWND hwnd, IDXGISwapChain* pSwapChain );
// SCITER -

//--------------------------------------------------------------------------------------
HANDLE gGLDeviceHandleD3DInterop = static_cast<HANDLE>(nullptr);
GLuint gGLBackBuffer = 0;
HANDLE gGLBackBufferHandleD3DInterop = static_cast<HANDLE>(nullptr);
GLuint g_GLTexture = 0;

void InitializeInterop( )
{
	HWND g_hEmptyWnd = ::CreateWindowA( "STATIC", NULL, CS_OWNDC, 0, 0, 0, 0, NULL, NULL, NULL, NULL );
	//HDC hdc = GetDC( g_hEmptyWnd );

	static	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof( PIXELFORMATDESCRIPTOR ),              // Size Of This Pixel Format Descriptor
		1,                                          // Version Number
		PFD_DRAW_TO_WINDOW |                        // Format Must Support Window
		PFD_SUPPORT_OPENGL |                        // Format Must Support OpenGL
		PFD_DOUBLEBUFFER,                           // Must Support Double Buffering
		PFD_TYPE_RGBA,                              // Request An RGBA Format
		32,                                         // Select Our Color Depth
		0, 0, 0, 0, 0, 0,                           // Color Bits Ignored
		0,                                          // No Alpha Buffer
		0,                                          // Shift Bit Ignored
		0,                                          // No Accumulation Buffer
		0, 0, 0, 0,                                 // Accumulation Bits Ignored
		16,                                         // 16Bit Z-Buffer (Depth Buffer)  
		0,                                          // No Stencil Buffer
		0,                                          // No Auxiliary Buffer
		PFD_MAIN_PLANE,                             // Main Drawing Layer
		0,                                          // Reserved
		0, 0, 0                                     // Layer Masks Ignored
	};

	HDC g_hDCGL = NULL;
	g_hDCGL = GetDC( g_hEmptyWnd );
	GLuint PixelFormat = ChoosePixelFormat( g_hDCGL, &pfd );
	//GLuint PixelFormat = GetPixelFormat( g_hDCGL );
	SetPixelFormat( g_hDCGL, PixelFormat, &pfd );
	HGLRC hRC = wglCreateContext( g_hDCGL );
	wglMakeCurrent( g_hDCGL, hRC );

	GLenum x = glewInit( );

	if (WGLEW_NV_DX_interop2)
	{
		// Register D3D11 device with GL
		gGLDeviceHandleD3DInterop = wglDXOpenDeviceNV( g_pd3dDevice );

		// register the Direct3D color buffer as renderbuffers in opengl
		glGenRenderbuffers( 1, &gGLBackBuffer );

		ID3D11Resource* pBackBuffer = NULL;
		HRESULT hr = g_pSwapChain->GetBuffer( 0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer );
		if (FAILED( hr ))
			return;

		gGLBackBufferHandleD3DInterop = wglDXRegisterObjectNV( gGLDeviceHandleD3DInterop,
			pBackBuffer,
			gGLBackBuffer,
			GL_RENDERBUFFER,
			WGL_ACCESS_READ_WRITE_NV );

		pBackBuffer->Release( );

	}
}

void DestroyInterop( )
{
	if (WGLEW_NV_DX_interop)
	{
		// Unregister the Direct3D color buffer as renderbuffers in opengl
		BOOL bResult = wglDXUnregisterObjectNV( gGLDeviceHandleD3DInterop, gGLBackBufferHandleD3DInterop );

		// Unregister D3D11 device with GL
		bResult = wglDXCloseDeviceNV( gGLDeviceHandleD3DInterop );
	}
}

//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT InitDevice( )
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect( g_hWnd, &rc );
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
	UINT numDriverTypes = ARRAYSIZE( driverTypes );

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE( featureLevels );

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof( sd ) );
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
		hr = D3D11CreateDeviceAndSwapChain( NULL, g_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext );
		if (SUCCEEDED( hr ))
			break;
	}
	if (FAILED( hr ))
		return hr;


	// SCITER+
		// create Sciter engine instance for the window:
	InitSciterEngineInstance( g_hWnd, g_pSwapChain );
	// SCITER-

	InitializeInterop( );

	return S_OK;
}

//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice( )
{
	DestroyInterop( );

	if (g_pImmediateContext) g_pImmediateContext->ClearState( );
	if (g_pSwapChain) g_pSwapChain->Release( );
	if (g_pImmediateContext) g_pImmediateContext->Release( );
	if (g_pd3dDevice) g_pd3dDevice->Release( );
}


void Resize( )
{
	if (!g_pSwapChain)
		return;

	RECT rc;
	GetClientRect( g_hWnd, &rc );

	int width = rc.right - rc.left;
	int height = rc.top - rc.bottom;

	if (WGLEW_NV_DX_interop)
	{
		// Unregister the Direct3D color buffer as renderbuffers in opengl
		BOOL bResult = wglDXUnregisterObjectNV( gGLDeviceHandleD3DInterop, gGLBackBufferHandleD3DInterop );
	}

	HRESULT hr;
	// Preserve the existing buffer count and format.
	// Automatically choose the width and height to match the client rect for HWNDs.
	hr = g_pSwapChain->ResizeBuffers( 0, 0, 0, DXGI_FORMAT_UNKNOWN, 0 );

	// Perform error handling here!

	// Get buffer and create a render-target-view.
	ComPtr<ID3D11Resource> pBackBuffer;
	hr = g_pSwapChain->GetBuffer( 0, MY_IID_PPV_ARGS( &pBackBuffer ) );
	// Perform error handling here!

	if (WGLEW_NV_DX_interop2)
	{
		gGLBackBufferHandleD3DInterop = wglDXRegisterObjectNV( gGLDeviceHandleD3DInterop,
			pBackBuffer.Get(),
			gGLBackBuffer,
			GL_RENDERBUFFER,
			WGL_ACCESS_READ_WRITE_NV );
	}
}

//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	//SCITER +
	  // mix-in Sciter functionality 
	if (message != WM_CREATE) // we create the engine instance later,
							   // by calling SciterCreateOnDirectXWindow(hWnd, pSwapChain);
	{
		BOOL handled = FALSE;
		LRESULT lr = SciterProcND( hWnd, message, wParam, lParam, &handled );
		if (handled)
			return lr;
	}
	//SCITER -

	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint( hWnd, &ps );
		EndPaint( hWnd, &ps );
		break;

	case WM_SIZE:
		Resize( );
		break;

	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;

	default:
		return DefWindowProc( hWnd, message, wParam, lParam );
	}

	return 0;
}

void RenderGL( )
{
	if (wglGetCurrentContext( ) == NULL)
		return;

	// Lock the shared surface
	wglDXLockObjectsNV( gGLDeviceHandleD3DInterop, 1, &gGLBackBufferHandleD3DInterop );

	GLuint hFBO = 0;
	glGenFramebuffers( 1, &hFBO );
	glBindFramebuffer( GL_FRAMEBUFFER, hFBO );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0 );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0 );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, gGLBackBuffer );

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	glBindFramebuffer( GL_FRAMEBUFFER, hFBO );

	glClearColor( 0.95f, 0.95f, 0.95f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );

	RECT rc;
	GetClientRect( g_hWnd, &rc );
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;
	glViewport( 0, 0, (GLsizei)width, (GLsizei)height );

	glDisable( GL_DEPTH_TEST );
	glDisable( GL_BLEND );

	glPushMatrix( );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	gluPerspective( 60, (GLfloat)width / (GLfloat)height, 1.0, 100.0 );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	glTranslatef( 0.0f, 0.0f, -5.0f );

	static float angle = 0.1f;
	angle += g_rotationSpeed;
	glRotatef( angle, 0.0f, 1.0f, 1.0f );

	glBegin( GL_QUADS );
	glColor3f( g_colorSpeed * 0.5f, 0.0f, 1.0f ); // make this vertex purple
	glVertex2f( -0.75, 0.75 );
	glColor3f( g_colorSpeed * 1.0f, 0.0f, 0.0f ); // make this vertex red
	glVertex2f( -0.75, -0.75 );
	glColor3f( 0.0f, g_colorSpeed * 1.0f, 0.0f ); // make this vertex green
	glVertex2f( 0.75, -0.75 );
	glColor3f( g_colorSpeed *1.0f, 1.0f, 0.0f ); // make this vertex yellow
	glVertex2f( 0.75, 0.75 );
	glEnd( );

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	glDeleteFramebuffers( 1, &hFBO );

	// Unlock the shared surface
	wglDXUnlockObjectsNV( gGLDeviceHandleD3DInterop, 1, &gGLBackBufferHandleD3DInterop );
}

//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
void Render( )
{
	RenderGL( );

	// SCITER +
		// render GUI layers on top (front layer)
	if (g_backLayer && g_foreLayer)
	{
		SciterRenderOnDirectXWindow( g_hWnd, g_backLayer, TRUE );
		SciterRenderOnDirectXWindow( g_hWnd, g_foreLayer, TRUE );
	}
	// SCITER -

	g_pSwapChain->Present( 0, 0 );
}

// SCITER +

  // view_dom_event_handler, provides native methods to be used in script

struct view_dom_event_handler : public sciter::event_handler {
	BEGIN_FUNCTION_MAP
		FUNCTION_1( "setRotationSpeed", setRotationSpeed )
		FUNCTION_1( "setColorSpeed", setColorSpeed )
	END_FUNCTION_MAP

	sciter::value setRotationSpeed( sciter::value speed )
	{
		g_rotationSpeed = (FLOAT)speed.get( 1.0 );
		return sciter::value( true );
	}
	sciter::value setColorSpeed( sciter::value speed )
	{
		g_colorSpeed = (FLOAT)speed.get( 1.0 );
		return sciter::value( true );
	}

};

view_dom_event_handler g_ViewDomEventHandler;

UINT SC_CALLBACK SciterCallback( LPSCITER_CALLBACK_NOTIFICATION pns, LPVOID callbackParam );

// create Sciter engine instance for the window:
BOOL InitSciterEngineInstance( HWND hWnd, IDXGISwapChain *pSwapChain )
{
	// 1. create engine instance on the window with the swap chain:
	BOOL r = SciterCreateOnDirectXWindow( hWnd, pSwapChain );
	if (!r) return FALSE;

	SciterSetOption( hWnd, SCITER_SET_DEBUG_MODE, FALSE );

	// 2. setup callback (resource loading, etc):
	SciterSetCallback( hWnd, SciterCallback, NULL );

	// 2b. setup DOM event handler:
	sciter::attach_dom_event_handler( hWnd, &g_ViewDomEventHandler );

	// 3. load HTML content in it:
	SciterLoadFile( hWnd, L"res:facade.htm" );
	//SciterLoadFile( hWnd, L"interop-proto.htm" );

	// 4. get layer elements:
	sciter::dom::element root = sciter::dom::element::root_element( hWnd );
	g_backLayer = root.find_first( "section#back-layer" );
	g_foreLayer = root.find_first( "section#fore-layer" );
	assert( g_backLayer && g_foreLayer );

	// done
	return true;
}

// handle SC_LOAD_DATA requests - get data from resources of this application
UINT DoLoadData( LPSCN_LOAD_DATA pnmld )
{
	LPCBYTE pb = 0; UINT cb = 0;
	aux::wchars wu = aux::chars_of( pnmld->uri );
	if (wu.like( WSTR( "res:*" ) ))
	{
		// then by calling possibly overloaded load_resource_data method
		if (sciter::load_resource_data( g_hInst, wu.start + 4, pb, cb ))
			::SciterDataReady( pnmld->hwnd, pnmld->uri, pb, cb );
	}
	else if (wu.like( WSTR( "this://app/*" ) )) {
		// try to get them from archive (if any, you need to call sciter::archive::open() first)
		aux::bytes adata = sciter::archive::instance( ).get( wu.start + 11 );
		if (adata.length)
			::SciterDataReady( pnmld->hwnd, pnmld->uri, adata.start, adata.length );
	}
	return LOAD_OK;
}

// fulfill SC_ATTACH_BEHAVIOR request 
UINT DoAttachBehavior( LPSCN_ATTACH_BEHAVIOR lpab )
{
	sciter::event_handler *pb = sciter::behavior_factory::create( lpab->behaviorName, lpab->element );
	if (pb)
	{
		lpab->elementTag = pb;
		lpab->elementProc = sciter::event_handler::element_proc;
		return TRUE;
	}
	return FALSE;
}

UINT SC_CALLBACK SciterCallback( LPSCITER_CALLBACK_NOTIFICATION pns, LPVOID callbackParam )
{
	// here are all notifiactions
	switch (pns->code)
	{
	case SC_LOAD_DATA:          return DoLoadData( (LPSCN_LOAD_DATA)pns );
	case SC_ATTACH_BEHAVIOR:    return DoAttachBehavior( (LPSCN_ATTACH_BEHAVIOR)pns );
	}
	return 0;
}
// SCITER -

//} //namespace SciterInterop

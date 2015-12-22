
// SciterChildView.cpp : implementation of the CSciterChildView class
//

#include "stdafx.h"
#include "sciter-mfc.h"
#include "SciterChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSciterChildView

CSciterChildView::CSciterChildView()
{
}

CSciterChildView::~CSciterChildView()
{
}


BEGIN_MESSAGE_MAP(CSciterChildView, CWnd)
	//ON_WM_PAINT()
  ON_WM_CREATE()
END_MESSAGE_MAP()



// CSciterChildView message handlers

BOOL CSciterChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}


// Sciter "mix-in" type of integration, see: http://sciter.com/developers/embedding-principles/
LRESULT CSciterChildView::WindowProc(UINT message,WPARAM wParam,LPARAM lParam)
{
  LRESULT lResult;
  BOOL    bHandled;

  lResult = SciterProcND( this->GetSafeHwnd(), message,wParam,lParam, &bHandled);
  if(bHandled)      // if it was handled by the Sciter
    return lResult; // then no further processing is required.

  return __super::WindowProc(message,wParam,lParam);
}

HWINDOW   CSciterChildView::get_hwnd() { return this->GetSafeHwnd(); }
HINSTANCE CSciterChildView::get_resource_instance() { return theApp.m_hInstance; } 


int CSciterChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (__super::OnCreate(lpCreateStruct) == -1)
    return -1;

  this->setup_callback(); // attach sciter::host callbacks
  sciter::attach_dom_event_handler(this->get_hwnd(),this); // attach this as a DOM events 

  // load intial document
  this->load_file(L"res:default.htm");

  return 0;
}

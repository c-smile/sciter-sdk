
// SciterChildView.h : interface of the CSciterChildView class
//


#pragma once

#include "sciter-x.h"  // sciter headers


// CSciterChildView window

class CSciterChildView : public CWnd, 
                         public sciter::host<CSciterChildView>, // Sciter host window primitives
                         public sciter::event_handler           // Sciter DOM event handling
{
// Construction
public:
	CSciterChildView();

// Attributes
public:

// Operations
public:

// Sciter support
  virtual LRESULT WindowProc(UINT message,WPARAM wParam,LPARAM lParam);
// Sciter support, sciter::host traits:
  HWINDOW   get_hwnd();
  HINSTANCE get_resource_instance();

// Sciter DOM event handlers, sciter::event_handler overridables 
  virtual bool handle_mouse  (HELEMENT he, MOUSE_PARAMS& params ) override         { return false; }
  virtual bool handle_key    (HELEMENT he, KEY_PARAMS& params ) override           { return false; }
  virtual bool handle_focus  (HELEMENT he, FOCUS_PARAMS& params )  override        { return false; }
  virtual bool handle_event (HELEMENT he, BEHAVIOR_EVENT_PARAMS& params ) override { return false; }
  virtual bool handle_method_call (HELEMENT he, METHOD_PARAMS& params )  override  { return false; }

// Overrides
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CSciterChildView();

	// Generated message map functions
protected:
	//afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};



// sciter-mfc.h : main header file for the sciter-mfc application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CSciterMfcApp:
// See sciter-mfc.cpp for the implementation of this class
//

class CSciterMfcApp : public CWinApp
{
public:
	CSciterMfcApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CSciterMfcApp theApp;

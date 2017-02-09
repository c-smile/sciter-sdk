// Sciter.h : Declaration of the CSciter
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include "AxSciter_i.h"
#include "_ISciterEvents_CP.h"
#include "sciter-x.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;

extern HINSTANCE ghInstance;


// CSciter
class ATL_NO_VTABLE CSciter :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<ISciter, &IID_ISciter, &LIBID_AxSciterLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IOleControlImpl<CSciter>,
	public IOleObjectImpl<CSciter>,
	public IOleInPlaceActiveObjectImpl<CSciter>,
	public IViewObjectExImpl<CSciter>,
	public IOleInPlaceObjectWindowlessImpl<CSciter>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CSciter>,
	public CProxy_ISciterEvents<CSciter>,
	public IQuickActivateImpl<CSciter>,
#ifndef _WIN32_WCE
	public IDataObjectImpl<CSciter>,
#endif
	public IProvideClassInfo2Impl<&CLSID_Sciter, &__uuidof(_ISciterEvents), &LIBID_AxSciterLib>,
	public CComCoClass<CSciter, &CLSID_Sciter>,
	public CComControl<CSciter>,
  // Sciter stuff
  public sciter::host<CSciter>,
  public sciter::event_handler

{
public:


	CSciter()
	{
	}

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_ACTSLIKEBUTTON |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_INSIDEOUT |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_SCITER)


BEGIN_COM_MAP(CSciter)
	COM_INTERFACE_ENTRY(ISciter)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IViewObjectEx)
	COM_INTERFACE_ENTRY(IViewObject2)
	COM_INTERFACE_ENTRY(IViewObject)
	COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceObject)
	COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
	COM_INTERFACE_ENTRY(IOleControl)
	COM_INTERFACE_ENTRY(IOleObject)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(IQuickActivate)
#ifndef _WIN32_WCE
	COM_INTERFACE_ENTRY(IDataObject)
#endif
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
END_COM_MAP()

BEGIN_PROP_MAP(CSciter)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY_TYPE("Property Name", dispid, clsid, vtType)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()

BEGIN_CONNECTION_POINT_MAP(CSciter)
	CONNECTION_POINT_ENTRY(__uuidof(_ISciterEvents))
END_CONNECTION_POINT_MAP()

BEGIN_MSG_MAP(CSciter)
  // delegate processing all messages to Sciter first:
  MESSAGE_RANGE_HANDLER(0, UINT(-1), SciterMsgDeleagate)
	CHAIN_MSG_MAP(CComControl<CSciter>)
	DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid)
	{
		static const IID* const arr[] =
		{
			&IID_ISciter,
		};

		for (int i=0; i<sizeof(arr)/sizeof(arr[0]); i++)
		{
			if (InlineIsEqualGUID(*arr[i], riid))
				return S_OK;
		}
		return S_FALSE;
	}

// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

// ISciter
public:
	HRESULT OnDraw(ATL_DRAWINFO& di)
	{
		RECT& rc = *(RECT*)di.prcBounds;
		// Set Clip region to the rectangle specified by di.prcBounds
		HRGN hRgnOld = NULL;
		if (GetClipRgn(di.hdcDraw, hRgnOld) != 1)
			hRgnOld = NULL;
		bool bSelectOldRgn = false;

		HRGN hRgnNew = CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);

		if (hRgnNew != NULL)
		{
			bSelectOldRgn = (SelectClipRgn(di.hdcDraw, hRgnNew) != ERROR);
		}

		Rectangle(di.hdcDraw, rc.left, rc.top, rc.right, rc.bottom);
		SetTextAlign(di.hdcDraw, TA_CENTER|TA_BASELINE);
		LPCTSTR pszText = _T("Sciter");
#ifndef _WIN32_WCE
		TextOut(di.hdcDraw,
			(rc.left + rc.right) / 2,
			(rc.top + rc.bottom) / 2,
			pszText,
			lstrlen(pszText));
#else
		ExtTextOut(di.hdcDraw,
			(rc.left + rc.right) / 2,
			(rc.top + rc.bottom) / 2,
			ETO_OPAQUE,
			NULL,
			pszText,
			ATL::lstrlen(pszText),
			NULL);
#endif

		if (bSelectOldRgn)
			SelectClipRgn(di.hdcDraw, hRgnOld);

		DeleteObject(hRgnNew);

		return S_OK;
	}

  // sciter::host traits
  HWINDOW   get_hwnd() { return m_hWnd; }
  HINSTANCE get_resource_instance() { return ghInstance; }

  LRESULT SciterMsgDeleagate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
  {
    LRESULT lr = SciterProcND(m_hWnd, uMsg, wParam, lParam, &bHandled);
    if (uMsg == WM_CREATE) {
      // bind CSciter instance with events/callbacks 
      setup_callback(); // to receive SC_LOAD_DATA, SC_DATA_LOADED, etc. notification
      attach_dom_event_handler(m_hWnd, this); // to receive DOM events
      //load blank document
      aux::chars blank_doc = const_chars("<html style='background:#ccc'><body></body></html>");
      load_html((LPCBYTE)blank_doc.start,blank_doc.length,L"about:blank");
    }
    return lr;
  }

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}
  STDMETHOD(LoadHtmlFile)(BSTR fileUrl, VARIANT_BOOL* result);
  STDMETHOD(LoadHtml)(BSTR html, BSTR baseUrl, VARIANT_BOOL* result);
};

OBJECT_ENTRY_AUTO(__uuidof(Sciter), CSciter)

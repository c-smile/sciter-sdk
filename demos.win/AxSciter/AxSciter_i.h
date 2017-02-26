

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Sun Feb 26 08:54:51 2017
 */
/* Compiler settings for AxSciter.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __AxSciter_i_h__
#define __AxSciter_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ISciter_FWD_DEFINED__
#define __ISciter_FWD_DEFINED__
typedef interface ISciter ISciter;
#endif 	/* __ISciter_FWD_DEFINED__ */


#ifndef ___ISciterEvents_FWD_DEFINED__
#define ___ISciterEvents_FWD_DEFINED__
typedef interface _ISciterEvents _ISciterEvents;
#endif 	/* ___ISciterEvents_FWD_DEFINED__ */


#ifndef __Sciter_FWD_DEFINED__
#define __Sciter_FWD_DEFINED__

#ifdef __cplusplus
typedef class Sciter Sciter;
#else
typedef struct Sciter Sciter;
#endif /* __cplusplus */

#endif 	/* __Sciter_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __ISciter_INTERFACE_DEFINED__
#define __ISciter_INTERFACE_DEFINED__

/* interface ISciter */
/* [unique][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ISciter;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CE8DB2CD-C37D-40D2-9E2A-911ED7A312CD")
    ISciter : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE LoadHtmlFile( 
            /* [in] */ BSTR fileUrl,
            /* [retval][out] */ VARIANT_BOOL *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadHtml( 
            /* [in] */ BSTR html,
            BSTR baseUrl,
            /* [retval][out] */ VARIANT_BOOL *result) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISciterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISciter * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISciter * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISciter * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISciter * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISciter * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISciter * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISciter * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *LoadHtmlFile )( 
            ISciter * This,
            /* [in] */ BSTR fileUrl,
            /* [retval][out] */ VARIANT_BOOL *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadHtml )( 
            ISciter * This,
            /* [in] */ BSTR html,
            BSTR baseUrl,
            /* [retval][out] */ VARIANT_BOOL *result);
        
        END_INTERFACE
    } ISciterVtbl;

    interface ISciter
    {
        CONST_VTBL struct ISciterVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISciter_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISciter_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISciter_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISciter_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISciter_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISciter_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISciter_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ISciter_LoadHtmlFile(This,fileUrl,result)	\
    ( (This)->lpVtbl -> LoadHtmlFile(This,fileUrl,result) ) 

#define ISciter_LoadHtml(This,html,baseUrl,result)	\
    ( (This)->lpVtbl -> LoadHtml(This,html,baseUrl,result) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISciter_INTERFACE_DEFINED__ */



#ifndef __AxSciterLib_LIBRARY_DEFINED__
#define __AxSciterLib_LIBRARY_DEFINED__

/* library AxSciterLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_AxSciterLib;

#ifndef ___ISciterEvents_DISPINTERFACE_DEFINED__
#define ___ISciterEvents_DISPINTERFACE_DEFINED__

/* dispinterface _ISciterEvents */
/* [uuid] */ 


EXTERN_C const IID DIID__ISciterEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("09C2F7D8-E1BD-4568-99A0-631D3A3A09A9")
    _ISciterEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _ISciterEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _ISciterEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _ISciterEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _ISciterEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _ISciterEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _ISciterEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _ISciterEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _ISciterEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _ISciterEventsVtbl;

    interface _ISciterEvents
    {
        CONST_VTBL struct _ISciterEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _ISciterEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _ISciterEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _ISciterEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _ISciterEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _ISciterEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _ISciterEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _ISciterEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___ISciterEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_Sciter;

#ifdef __cplusplus

class DECLSPEC_UUID("7832863F-C30A-49DF-B65E-3B1CE46C829E")
Sciter;
#endif
#endif /* __AxSciterLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



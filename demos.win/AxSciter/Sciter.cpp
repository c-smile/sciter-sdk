// Sciter.cpp : Implementation of CSciter
#include "stdafx.h"
#include "Sciter.h"


// CSciter

STDMETHODIMP CSciter::LoadHtmlFile(BSTR fileUrl, VARIANT_BOOL* result)
{
  *result = SciterLoadFile(m_hWnd, fileUrl);
  return S_OK;
}

STDMETHODIMP CSciter::LoadHtml(BSTR html, BSTR baseUrl, VARIANT_BOOL* result)
{
  aux::w2utf utf8(html);
  *result = SciterLoadHtml(m_hWnd, utf8, UINT(utf8.length()), baseUrl);
  return S_OK;
}

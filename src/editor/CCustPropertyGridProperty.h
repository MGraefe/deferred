// editor/CCustPropertyGridProperty.h
// editor/CCustPropertyGridProperty.h
// editor/CCustPropertyGridProperty.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__editor__CCustPropertyGridProperty_H__
#define deferred__editor__CCustPropertyGridProperty_H__

#include <afxpropertygridctrl.h>

class CCustPropertyGridProperty : public CMFCPropertyGridProperty
{
	friend class CMFCPropertyGridCtrl;

// Construction
public:
	// Group constructor
	CCustPropertyGridProperty(const CString& strGroupName, DWORD_PTR dwData = 0, BOOL bIsValueList = FALSE) :
		CMFCPropertyGridProperty(strGroupName, dwData, bIsValueList) { }

	// Simple property
	CCustPropertyGridProperty(const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
		LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL, LPCTSTR lpszValidChars = NULL) :
		CMFCPropertyGridProperty(strName, varValue, lpszDescr, dwData, lpszEditMask, lpszEditTemplate, lpszValidChars) { }

	CWnd *GetInPlaceWnd() { return m_pWndInPlace; }
};

#endif

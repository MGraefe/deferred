// editor/CustColorProperty.h
// editor/CustColorProperty.h
// editor/CustColorProperty.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

//CustColorProperty.h

#pragma once
#ifndef deferred__editor__CustColorProperty_H__
#define deferred__editor__CustColorProperty_H__

#include "CCustPropertyGridProperty.h"

class CCustColorProperty : public CMFCPropertyGridColorProperty
{
	friend class CMFCPropertyGridCtrl;
	//DECLARE_DYNAMIC(CCustColorProperty)

	typedef CMFCPropertyGridColorProperty BaseClass;

public:
	CCustColorProperty(const CString& strName, const COLORREF& color, float brightness, CPalette* pPalette = NULL, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);
	void OnClickButton(CPoint cpoint);
	CString FormatProperty();
	BOOL OnUpdateValue();
	BOOL OnEdit(LPPOINT lptClick);
	void SetColor(COLORREF color, float brightness);

private:
	static UINT_PTR CALLBACK CCHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam );
	void OnChangeColorHot(COLORREF color);
	BOOL OpenColorDialog(const COLORREF colorDefault, COLORREF& colorRes);

private:
	static COLORREF customColors[16];
	bool m_bColorDialogOpen;
	HWND m_colorDialog;
	float m_brightness;
	float m_oldBrightness;
};

#endif

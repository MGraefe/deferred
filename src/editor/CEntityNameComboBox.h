// editor/CEntityNameComboBox.h
// editor/CEntityNameComboBox.h
// editor/CEntityNameComboBox.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__editor__CEntityNameComboBox_H__
#define deferred__editor__CEntityNameComboBox_H__



class CEntityNameSubListBox : public CListBox
{
public:
	//virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
};


#define CCB_NOWEIGHT 1
#define CCB_NOCOLOR 2

class CCustomComboBox : public CComboBox
{
public:
	void SetFontData(bool bold, COLORREF color, UINT flags = 0);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	CEdit m_edit;
	CEntityNameSubListBox m_listbox;

protected:
	afx_msg virtual HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg virtual void OnDestroy();

private:
	CFont m_editFontNormal;
	CFont m_editFontThick;
	COLORREF m_editTextColor;
public:
	DECLARE_MESSAGE_MAP()
};

class CEntityNameComboBox : public CCustomComboBox
{
public:
	void FillData();
};

#endif // deferred__editor__CEntityNameComboBox_H__
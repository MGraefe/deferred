// editor/CustColorProperty.cpp
// editor/CustColorProperty.cpp
// editor/CustColorProperty.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#include "stdafx.h"
#include "CustColorProperty.h"
#include "PropertiesWnd.h"

COLORREF CCustColorProperty::customColors[16] = {0};

CCustColorProperty::CCustColorProperty(const CString& strName, const COLORREF& color, float brightness, CPalette* pPalette, LPCTSTR lpszDescr, DWORD_PTR dwData) :
	CMFCPropertyGridColorProperty(strName, color, pPalette, lpszDescr, dwData)
{
	m_bColorDialogOpen = false;
	m_Color = color;
	m_oldBrightness = m_brightness = brightness;
	m_varValue = FormatProperty();
	m_varValueOrig = FormatProperty();
}


std::map<HWND, CCustColorProperty*> hookMap;
UINT_PTR CALLBACK CCustColorProperty::CCHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	if(uiMsg == WM_INITDIALOG)
	{
		CHOOSECOLOR *cc = (CHOOSECOLOR*)lParam;
		CCustColorProperty *parent = (CCustColorProperty*)cc->lCustData;
		hookMap[hdlg] = parent;
		::SetWindowText(hdlg, parent->GetName());
		parent->m_colorDialog = hdlg;
	}
	else if(uiMsg == WM_DESTROY)
	{
		hookMap.erase(hdlg);
	}
	else if(uiMsg == WM_CTLCOLORSTATIC)
	{
		TCHAR szR[256];
		szR[0] = 0;
		::GetDlgItemText(hdlg, 0x2c2, szR, 256);
		TCHAR szG[256];
		szG[0] = 0;
		::GetDlgItemText(hdlg, 0x2c3, szG, 256);
		TCHAR szB[256];
		szB[0] = 0;
		::GetDlgItemText(hdlg, 0x2c4, szB, 256);

		COLORREF color = RGB(_ttoi(szR), _ttoi(szG), _ttoi(szB));
		
		if(hookMap.find(hdlg) != hookMap.end())
			hookMap[hdlg]->OnChangeColorHot(color);
	}

	return 0;
}


BOOL CCustColorProperty::OpenColorDialog(const COLORREF colorDefault, COLORREF& colorRes)
{
	CHOOSECOLOR cc;
	memset(&cc, 0, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = NULL;
	cc.lCustData = (LPARAM)(this);
	cc.lpCustColors = customColors;
	cc.lpfnHook = &CCHookProc;
	cc.rgbResult = colorDefault;
	cc.Flags = CC_ANYCOLOR | CC_ENABLEHOOK | CC_FULLOPEN | CC_RGBINIT;

	m_bColorDialogOpen = true;
	BOOL result = ::ChooseColor(&cc);
	m_bColorDialogOpen = false;

	if(result)
		colorRes = cc.rgbResult;
	else
		colorRes = colorDefault;

	return result;
}


void CCustColorProperty::OnChangeColorHot(COLORREF color)
{
	this->SetColor(color, m_brightness);
	CPropertiesWnd *propWnd = dynamic_cast<CPropertiesWnd*>(m_pWndList->GetParent());
	Assert(propWnd);
	propWnd->UpdateProperty(this, true);
}


void CCustColorProperty::OnClickButton(CPoint cpoint)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndList);

	if( m_bColorDialogOpen )
	{
		::SetFocus(m_colorDialog);
	}
	else
	{
		m_bButtonIsDown = TRUE;
		Redraw();

		COLORREF oldColor = m_Color;
		COLORREF result = (COLORREF)-1;
		OpenColorDialog(GetData() == 0 ? this->GetColor() : RGB(0, 0, 0), result);
		this->SetColor(result, m_brightness);

		//do an additional update
		if(result == oldColor)
		{
			CPropertiesWnd *propWnd = dynamic_cast<CPropertiesWnd*>(m_pWndList->GetParent());
			Assert(propWnd);
			propWnd->UpdateProperty(this, false);
		}
	}

	//CList<COLORREF,COLORREF> lstDocColors;

	//m_pPopup = new CMFCColorPopupMenu(NULL, m_Colors, m_Color, NULL, NULL, NULL, lstDocColors, m_nColumnsNumber, m_ColorAutomatic);
	//m_pPopup->SetPropList(m_pWndList);

	//if (!m_strOtherColor.IsEmpty()) // Other color button
	//{
	//	m_pPopup->m_wndColorBar.EnableOtherButton(m_strOtherColor, !m_bStdColorDlg);
	//}

	//if (!m_strAutoColor.IsEmpty()) // Automatic color button
	//{
	//	m_pPopup->m_wndColorBar.EnableAutomaticButton(m_strAutoColor, m_ColorAutomatic);
	//}

	//CPoint pt(m_pWndList->m_rectList.left + m_pWndList->m_nLeftColumnWidth + 1, m_rectButton.bottom + 1);
	//m_pWndList->ClientToScreen(&pt);

	//if (!m_pPopup->Create(m_pWndList, pt.x, pt.y, NULL, FALSE))
	//{
	//	ASSERT(FALSE);
	//	m_pPopup = NULL;
	//}
	//else
	//{
	//	m_pPopup->GetMenuBar()->SetFocus();
	//}
}


CString CCustColorProperty::FormatProperty()
{
	ASSERT_VALID(this);

	if (m_Color == (COLORREF)-1)
	{
		return m_strAutoColor;
	}

	CString str;

	if( GetData() != 1 )
		str.Format(_T("%i %i %i %.3f"), GetRValue(m_Color), GetGValue(m_Color), GetBValue(m_Color), m_brightness);
	else
		str = _T("(multiple values)");

	return str;
}


BOOL CCustColorProperty::OnEdit(LPPOINT lptClick)
{
	BOOL result = BaseClass::OnEdit(lptClick);
	CMFCMaskedEdit *edit = (CMFCMaskedEdit*)m_pWndInPlace;
	edit->SetValidChars(_T(" .0123456789"));
	edit->SetLimitText(32);
	if( GetData() != 1 )
		edit->SetWindowText(FormatProperty());
	return result;
}


BOOL CCustColorProperty::OnUpdateValue()
{
	ASSERT_VALID(this);

	if (m_pWndInPlace == NULL)
	{
		return FALSE;
	}

	ASSERT_VALID(m_pWndInPlace);
	ASSERT(::IsWindow(m_pWndInPlace->GetSafeHwnd()));

	CString strText;
	m_pWndInPlace->GetWindowText(strText);

	COLORREF colorCurr = m_Color;
	float curBrightness = m_brightness;

	if (!strText.IsEmpty())
	{
		int nR = 0, nG = 0, nB = 0;
		_stscanf_s(strText, _T("%i %i %i %f"), &nR, &nG, &nB, &m_brightness);
		nR = clamp(nR, 0, 255);
		nG = clamp(nG, 0, 255);
		nB = clamp(nB, 0, 255);
		m_Color = RGB(nR, nG, nB);
		m_varValue = FormatProperty();
	}

	if (colorCurr != m_Color || curBrightness != m_brightness)
	{
		m_pWndList->OnPropertyChanged(this);
	}

	return TRUE;
}

void CCustColorProperty::SetColor(COLORREF color, float brightness)
{
	BaseClass::SetColor(color);

	m_brightness = brightness;
	m_varValue = FormatProperty();
}

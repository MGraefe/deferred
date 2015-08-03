// editor/CEntityNameComboBox.cpp
// editor/CEntityNameComboBox.cpp
// editor/CEntityNameComboBox.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#include "stdafx.h"

#include "CEntityNameComboBox.h"
#include "editorDoc.h"
#include "CEditorEntityList.h"
#include "CEditorEntity.h"
#include <util/OutputSystem.h>

BEGIN_MESSAGE_MAP(CCustomComboBox, CComboBox)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()



HBRUSH CCustomComboBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CComboBox::OnCtlColor(pDC, pWnd, nCtlColor);
	if (nCtlColor == CTLCOLOR_EDIT)
	{
		//Edit control
		if (m_edit.GetSafeHwnd() == NULL)
		{
			m_edit.SubclassWindow(pWnd->GetSafeHwnd());
			CFont *font = m_edit.GetFont();						Assert(font);
			LOGFONT lfNorm, lfThick;
			int res;
			res = font->GetLogFont(&lfNorm);					Assert(res);
			res = font->GetLogFont(&lfThick);					Assert(res);
			lfThick.lfWeight = (LONG)((float)lfNorm.lfWeight * 1.5f + 0.5f);
			res = m_editFontNormal.CreateFontIndirect(&lfNorm); Assert(res);
			res = m_editFontThick.CreateFontIndirect(&lfThick); Assert(res);
			m_editTextColor = RGB(0, 0, 0);
		}
		pDC->SetTextColor(m_editTextColor);
	}
	else if (nCtlColor == CTLCOLOR_LISTBOX)
	{
		//ListBox control
		if (m_listbox.GetSafeHwnd() == NULL)
			m_listbox.SubclassWindow(pWnd->GetSafeHwnd());
	}

	return hbr;
}


void CCustomComboBox::OnDestroy()
{
	if (m_edit.GetSafeHwnd() != NULL)
		m_edit.UnsubclassWindow();
	if (m_listbox.GetSafeHwnd() != NULL)
		m_listbox.UnsubclassWindow();
	CComboBox::OnDestroy();
}


void CCustomComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	Assert(lpDrawItemStruct->CtlType == ODT_COMBOBOX);
	LPCTSTR lpszText = (LPCTSTR) lpDrawItemStruct->itemData;
	Assert(lpszText != NULL);
	CDC dc;

	dc.Attach(lpDrawItemStruct->hDC);

	// Save these value to restore them when done drawing.
	COLORREF crOldTextColor = dc.GetTextColor();
	COLORREF crOldBkColor = dc.GetBkColor();

	// If this item is selected, set the background color 
	// and the text color to appropriate values. Erase
	// the rect by filling it with the background color.
	if ((lpDrawItemStruct->itemAction & ODA_SELECT) &&
		(lpDrawItemStruct->itemState  & ODS_SELECTED))
	{
		dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		dc.SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
		dc.FillSolidRect(&lpDrawItemStruct->rcItem, ::GetSysColor(COLOR_HIGHLIGHT));
	}
	else
	{
		dc.FillSolidRect(&lpDrawItemStruct->rcItem, crOldBkColor);
	}

	// Draw the text.
	dc.DrawText(
		lpszText,
		(int)_tcslen(lpszText),
		&lpDrawItemStruct->rcItem,
		DT_CENTER|DT_SINGLELINE|DT_VCENTER);

	// Reset the background color and the text color back to their
	// original values.
	dc.SetTextColor(crOldTextColor);
	dc.SetBkColor(crOldBkColor);

	dc.Detach();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CCustomComboBox::SetFontData(bool bold, COLORREF color, UINT flags /*= 0*/)
{
	if(!(flags & CCB_NOCOLOR))
		m_editTextColor = color;

	if(IsWindow(m_edit))
	{
		if(!(flags & CCB_NOWEIGHT))
			m_edit.SetFont(bold ? &m_editFontThick : &m_editFontNormal);
		else
			m_edit.RedrawWindow();
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityNameComboBox::FillData()
{
	ResetContent();
	CeditorDoc *doc = GetActiveDocument();
	if(doc)
	{
		for(UINT i = 0; i < OutputSystem::TARGET_COUNT; i++)
			AddString(OutputSystem::getGenericTargetName((OutputSystem::genericTarget_e)i));

		auto ents = doc->GetEntityList()->GetVector();
		for(const CEditorEntity *ent : ents)
		{
			if(!ent->GetName().empty())
			{
				TCHAR *wide = CA2T(ent->GetName().c_str());
				if( FindString(-1, wide) == CB_ERR )
					AddString(wide);
			}
		}
	}
}

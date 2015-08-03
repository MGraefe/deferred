// starter/CMaskedEditCtrl.cpp
// starter/CMaskedEditCtrl.cpp
//CMaskedEditCtrl.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#include "stdafx.h"

#include "CMaskedEditCtrl.h"


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CMaskedEditCtrl::CMaskedEditCtrl() :
	CEdit(),
	m_mask(),
	m_inverse(false)
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CMaskedEditCtrl::CMaskedEditCtrl( const std::set<TCHAR> &mask, bool inverse /*= false*/ ) :
	CEdit(),
	m_mask(mask),
	m_inverse(inverse)
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
const std::set<TCHAR> & CMaskedEditCtrl::GetMask() const
{
	return m_mask;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CMaskedEditCtrl::SetMask( const std::set<TCHAR> &mask )
{
	m_mask = mask;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CMaskedEditCtrl::CharValid( TCHAR ch ) const
{
	return (ch >= 0 && ch < 32) || (m_mask.find(ch) == m_mask.end()) == m_inverse;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
BOOL CMaskedEditCtrl::PreTranslateMessage( MSG* pMsg )
{
	if(pMsg->message == WM_CHAR && !CharValid((TCHAR)pMsg->wParam))
	{
		MessageBeep(MB_OK);
		return TRUE;
	}
	return CEdit::PreTranslateMessage(pMsg);
}

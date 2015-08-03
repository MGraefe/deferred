// starter/CMaskedEditCtrl.h
// starter/CMaskedEditCtrl.h
//CMaskedEditCtrl.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once

#include "afxwin.h"
#include <set>

class CMaskedEditCtrl : public CEdit
{
public:
	CMaskedEditCtrl();
	CMaskedEditCtrl(const std::set<TCHAR> &mask, bool inverse = false);

	BOOL PreTranslateMessage(MSG* pMsg) override;

	void SetMask(const std::set<TCHAR> &mask);
	const std::set<TCHAR> &GetMask() const;

	void SetInverse(bool inverse) {
		m_inverse = inverse;
	}

	bool IsInverse() const {
		return m_inverse;
	}

	bool CharValid(TCHAR ch) const;

private:
	std::set<TCHAR> m_mask;
	bool m_inverse;
};
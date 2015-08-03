// editor/CToolsWndPane.h
// editor/CToolsWndPane.h
// editor/CToolsWndPane.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once

#include "CToolsWnd.h"

class CToolsWndPane : public CDockablePane
{

public: 
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CToolsWnd *GetToolsWnd( void ) { return &m_toolsWnd; }

	DECLARE_MESSAGE_MAP()

private:
	CToolsWnd m_toolsWnd;
};

// editor/CToolsWndPane.cpp
// editor/CToolsWndPane.cpp
// editor/CToolsWndPane.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"
#include "CToolsWndPane.h"


BEGIN_MESSAGE_MAP(CToolsWndPane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	//ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	//ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	//ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	//ON_COMMAND(ID_PROPERTIES1, OnProperties1)
	//ON_UPDATE_COMMAND_UI(ID_PROPERTIES1, OnUpdateProperties1)
	//ON_COMMAND(ID_PROPERTIES2, OnProperties2)
	//ON_UPDATE_COMMAND_UI(ID_PROPERTIES2, OnUpdateProperties2)
	//ON_WM_SETFOCUS()
	//ON_WM_SETTINGCHANGE()
END_MESSAGE_MAP()


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int CToolsWndPane::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if( CDockablePane::OnCreate( lpCreateStruct ) == -1 )
		return -1;

	m_toolsWnd.Create( IDD_TOOLS_WND, this );
	m_toolsWnd.ShowWindow(SW_SHOW);

	return 0;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolsWndPane::OnSize( UINT nType, int cx, int cy )
{
	m_toolsWnd.MoveWindow(0,0,cx,cy);
}


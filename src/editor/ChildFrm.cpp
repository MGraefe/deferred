// editor/ChildFrm.cpp
// editor/ChildFrm.cpp
// editor/ChildFrm.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "editor.h"

#include "ChildFrm.h"
#include "editorDoc.h"
#include "COpenGLView.h"
#include "CGLResources.h"
#include "C3dView.h"
#include "C2dView.h"
#include "OutputWnd.h"
#include "MainFrm.h"
#include "CEditorEntityList.h"
#include "CEditorEntity.h"
#include <util/CThreadMutex.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWndEx)
	ON_COMMAND(ID_WORLD_LOADWORLDFROM, &CChildFrame::OnOpenWorldProperties)
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_ACTIVATE()
	ON_WM_MOUSEACTIVATE()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_COMMAND(ID_EDIT_COPY, &CChildFrame::OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, &CChildFrame::OnEditPaste)
END_MESSAGE_MAP()

UINT GetGraphicsUpdateInterval()
{
	return 20;
}

// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	// TODO: add member initialization code here
	m_pGLResources = NULL;
	m_timerId = 0;
}

CChildFrame::~CChildFrame()
{
	if( m_pGLResources )
		delete m_pGLResources;
	//if( m_timerId != 0 )
	//	KillTimer(m_timerId);
}


//---------------------------------------------------------------

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs
	cs.style = WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
		| FWS_ADDTOTITLE | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE;

	if( !CMDIChildWndEx::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWndEx::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWndEx::Dump(dc);
}
#endif //_DEBUG



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CChildFrame::MaximizeView(int row, int col)
{
	CRect rect; 
	this->GetParent()->GetClientRect(&rect);

	m_SplitterWnd.SetRowInfo( 0, row == 0 ? rect.Height() : 0, 0 );
	m_SplitterWnd.SetColumnInfo( 0, col == 0 ? rect.Width() : 0, 0 );
	m_SplitterWnd.SetRowInfo( 1, row == 1 ? rect.Height() : 0, 0 );
	m_SplitterWnd.SetColumnInfo( 1, col == 1 ? rect.Width() : 0, 0 );
	m_SplitterWnd.RecalcLayout();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CChildFrame::ReorderViews()
{
	CRect rect;
	this->GetParent()->GetClientRect( &rect );
	int w = rect.Width() / 2;
	int h = rect.Height() / 2;

	m_SplitterWnd.SetRowInfo( 0, w, 0 );
	m_SplitterWnd.SetColumnInfo( 0, h, 0 );
	m_SplitterWnd.SetRowInfo( 1, w, 0 );
	m_SplitterWnd.SetColumnInfo( 1, h, 0 );
	m_SplitterWnd.RecalcLayout();
}

#define TIMER_ID 1681

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int CChildFrame::OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext* pContext )
{
	m_pGLResources = new CGLResources();
	BOOL ok = m_SplitterWnd.CreateStatic( this, 2, 2 );

	CreateSplitterView( 0, 0, pContext );
	CreateSplitterView( 0, 1, pContext );
	CreateSplitterView( 1, 0, pContext );
	CreateSplitterView( 1, 1, pContext );

	MaximizeView(0,0);

	// activate the input view
	SetActiveView( (CView*)m_SplitterWnd.GetPane(0,0) );

	Assert(m_timerId == 0);
	m_timerId = SetTimer(TIMER_ID, GetGraphicsUpdateInterval(), NULL);
	Assert(m_timerId != 0);

	return TRUE;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CChildFrame::CreateSplitterView( int row, int col, CCreateContext* pContext )
{
	Assert( (row == 0 || row == 1) && (col == 0 || col == 1 ) );
	RECT cltRect;
	this->GetParent()->GetClientRect( &cltRect );
	int midx = (cltRect.right-cltRect.left) / 2;
	int midy = (cltRect.bottom-cltRect.top) / 2;
	
	g_bCreate3dView = row == 0 && col == 0;
	m_SplitterWnd.CreateView( row, col, pContext->m_pNewViewClass, CSize(midx,midy), pContext );

	COpenGLView *pGlView = (COpenGLView*)(m_SplitterWnd.GetPane(row,col));
	pGlView->SetGLResources( m_pGLResources );
	pGlView->InitOpenGL( m_pGLResources );
	if( row != 0 || col != 0 )
	{
		pGlView->SetScrollRange(SB_HORZ, -10000, 10000, FALSE );
		pGlView->SetScrollRange(SB_VERT, -10000, 10000, FALSE );
		pGlView->SetScrollPos( SB_HORZ, 0 );
		pGlView->SetScrollPos( SB_VERT, 0 );
	}


	IRenderView *pView = NULL;

	if( row == 0 && col == 0 ) //top left
		pView = new C3dView();
	else if( row == 0 && col == 1 ) //top right
		pView = new C2dView( VIEW_TOP );
	else if( row == 1 && col == 0 ) //bot-left
		pView = new C2dView( VIEW_FRONT );
	else if( row == 1 && col == 1 ) //bot-right
		pView = new C2dView( VIEW_RIGHT );

	pGlView->AttachView(pView);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
afx_msg void CChildFrame::OnOpenWorldProperties()
{
	CeditorDoc *doc = (CeditorDoc*)GetActiveDocument();
	CEditorEntity *pWorld = NULL;
	const std::vector<CEditorEntity*> &ents = doc->GetEntityList()->GetVector();
	for( size_t i = 0; i < ents.size(); i++ )
	{
		if( ents.at(i)->GetClassnameA().compare("world") == 0 )
		{
			pWorld = ents[i];
			break;
		}
	}

	if( !pWorld )
	{
		error("No world existent.");
		return;
	}

	doc->SelectSingleEntity(pWorld);
	((CMainFrame*)AfxGetMainWnd())->OnOpenEntityProperties();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
COpenGLView * CChildFrame::GetView( viewtypes_e viewtype )
{
	for( int row = 0; row < m_SplitterWnd.GetRowCount(); row++ )
	{
		for( int col = 0; col < m_SplitterWnd.GetColumnCount(); col++ )
		{
			COpenGLView *pView = (COpenGLView*)m_SplitterWnd.GetPane(row,col);
			if(pView->GetViewType() == viewtype)
				return pView;
		}
	}
	return NULL;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CChildFrame * GetActiveChildFrame( void )
{
	return (CChildFrame*)((CMainFrame*)AfxGetMainWnd())->MDIGetActive();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CGLResources * GetActiveGLResources( void )
{
	CChildFrame *pChldFrm = GetActiveChildFrame();
	if( !pChldFrm )
		return NULL;
	return pChldFrm->GetGLResources();
}


void CChildFrame::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CMDIChildWndEx::OnWindowPosChanged(lpwndpos);
}


void CChildFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	if( nState == WA_ACTIVE || nState == WA_CLICKACTIVE )
	{
		m_pGLResources->SetActive( GetView(VIEW_3D)->GetDC()->m_hDC );
		theApp.GetMainFrame()->GetPropertiesWnd()->UpdateEntitySelection((CeditorDoc*)GetView(VIEW_3D)->GetDocument());
	}

	CMDIChildWndEx::OnActivate(nState, pWndOther, bMinimized);	
}


int CChildFrame::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	m_pGLResources->SetActive( GetView(VIEW_3D)->GetDC()->m_hDC );
	theApp.GetMainFrame()->GetPropertiesWnd()->UpdateEntitySelection();
	return CMDIChildWndEx::OnMouseActivate(pDesktopWnd, nHitTest, message);
}


void CChildFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	CMDIChildWndEx::OnUpdateFrameTitle(bAddToTitle);

	CDocument *doc = GetActiveDocument();
	if( doc && doc->IsModified() )
	{
		CString title;
		GetWindowText(title);
		title += "*";
		SetWindowText(title);
	}
}


void CChildFrame::OnClose()
{
	theApp.GetMainFrame()->GetPropertiesWnd()->UpdateEntitySelection();
	CMDIChildWndEx::OnClose();
}

CThreadMutex g_renderMutex;

void CChildFrame::OnTimer(UINT_PTR nIDEvent)
{
	if(GetActiveChildFrame() == this)
	{
		g_renderMutex.SetOrWait();
		m_pGLResources->SetActive( GetView(VIEW_3D)->GetDC()->m_hDC );
		m_pGLResources->UpdateAndDraw();
		CMDIChildWndEx::OnTimer(nIDEvent);
		g_renderMutex.Release();
	}
}


void CChildFrame::OnEditCopy()
{
	CeditorDoc *doc = (CeditorDoc*)GetActiveDocument();
	if(!doc)
		return;

	const SelectedEntityList &list = doc->GetSelectedEntityList();
	if(list.empty())
		return;

	//serialize to ostringstream
	std::ostringstream str;
	for(const CEditorEntity *ent : list)
		ent->Serialize(str);
	size_t strLen = str.str().length();

	//put into ole data source
	COleDataSource *oleSource = new COleDataSource;
	HGLOBAL glob = GlobalAlloc(GMEM_MOVEABLE, strLen + 1);
	char *chGlob = (char*)GlobalLock(glob);
	memcpy(chGlob, str.str().c_str(), strLen + 1);
	GlobalUnlock(glob);

	oleSource->CacheGlobalData(theApp.GetEntityClipboardFormat(), glob);
	oleSource->SetClipboard();
}


void CChildFrame::OnEditPaste()
{
	CeditorDoc *doc = (CeditorDoc*)GetActiveDocument();
	if(!doc)
		return;

	COleDataObject oleObj;
	if(!oleObj.AttachClipboard())
		return;
	
	if(!oleObj.IsDataAvailable(theApp.GetEntityClipboardFormat()))
	{
		MessageBeep(MB_OK);
		return;
	}

	HGLOBAL glob = oleObj.GetGlobalData(theApp.GetEntityClipboardFormat());
	if(!glob)
	{
		error("Paste error: memory error");
		return;
	}
	
	const char *chGlob = (const char*)GlobalLock(glob);
	size_t size = GlobalSize(glob);
	if(size == 0)
	{
		GlobalUnlock(glob);
		error("Paste error: zero sized memory");
		return;
	}

	CScriptParser parser;
	if(!parser.ParseFileString(chGlob))
	{
		GlobalUnlock(glob);
		error("Paste error: can't interpret data");
		return;
	}
	GlobalUnlock(glob);

	doc->UnselectAllEntities();
	std::vector<CEditorEntity*> addedEnts;
	doc->GetEntityList()->Deserialize(&parser, &addedEnts);

	doc->SelectAdditionalEntities(addedEnts.begin(), addedEnts.end());

	//Move entities to camera
	CCamera *cam = GetActiveCamera();
	if(cam)
	{
		Vector3f oldCenter = CEditorEntity::GetEntitiesCenter(addedEnts.begin(), addedEnts.end());
		Vector3f newCenter = cam->GetAbsPos() + cam->GetAbsAngles()*Vector3f(0.0f,0.0f,-1.0f)*10.0f;
		Vector3f dist = newCenter - oldCenter;
		for(CEditorEntity *ent : addedEnts)
			ent->SetAbsCenter(ent->GetAbsCenter() + dist);
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CCamera *CChildFrame::GetActiveCamera()
{
	COpenGLView *glview = GetView(VIEW_3D);
	if(glview)
	{
		C3dView *c3dview = (C3dView*)glview->GetRenderView();
		if(c3dview)
			return c3dview->GetCamera();
	}

	return NULL;
}

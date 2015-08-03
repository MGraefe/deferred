// editor/COpenGLView.cpp
// editor/COpenGLView.cpp
// editor/COpenGLView.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "editor.h"
#endif

#include "editorDoc.h"
#include "COpenGLView.h"
#include "CGLResources.h"

#include <gl/GL.h>
#include <gl/GLU.h>
#include "IRenderView.h"
#include "IEditorTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//HGLRC g_OglRenderCtx = NULL;
bool g_bCreate3dView = false;

// COpenGLView

IMPLEMENT_DYNCREATE(COpenGLView, CView)
//CObject* PASCAL COpenGLView::CreateObject()
//{
//	COpenGLView *pView = new COpenGLView();
//	pView->InitOpenGL();
//	return (CObject*)pView;
//}

//IMPLEMENT_RUNTIMECLASS(COpenGLView, CView, 0xFFFF, \
//	COpenGLView::CreateObject, NULL);

BEGIN_MESSAGE_MAP(COpenGLView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

// COpenGLView construction/destruction

COpenGLView::COpenGLView()
{
	// TODO: add construction code here
	m_bInitialized = false;
	m_RenderView = NULL;
	m_pGLResources = NULL;
}

COpenGLView::~COpenGLView()
{
	delete m_RenderView;
}

// Override the erase background function to
// do nothing to prevent flashing.
BOOL COpenGLView::OnEraseBkgnd(CDC* pDC) 
{	
	return TRUE;
}

BOOL COpenGLView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	//InitOpenGL();
	cs.lpszClass = ::AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC,
		::LoadCursor(NULL, IDC_ARROW), NULL, NULL);
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if( !g_bCreate3dView )
		cs.style |= WS_HSCROLL | WS_VSCROLL;

	return CView::PreCreateWindow(cs);
}

// COpenGLView drawing

void COpenGLView::OnDraw(CDC* pDC)
{
	CeditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;	

	m_pGLResources->SetActive( GetDC()->m_hDC );
	if( m_RenderView )
		m_RenderView->Render();
}

void COpenGLView::OnRButtonUp(UINT  nFlags , CPoint point)
{
	GetDocument()->GetActiveTool()->OnRButtonUp(this, nFlags, point);
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void COpenGLView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	//theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// COpenGLView diagnostics

#ifdef _DEBUG
void COpenGLView::AssertValid() const
{
	CView::AssertValid();
}

void COpenGLView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CeditorDoc* COpenGLView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CeditorDoc)));
	return (CeditorDoc*)m_pDocument;
}

#endif //_DEBUG



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int COpenGLView::InitOpenGL( CGLResources *pResources )
{
	m_pGLResources = pResources;

	HDC hdc = GetDC()->m_hDC;

	CRect rect;
	GetClientRect(&rect);
	pResources->Init(hdc, GetSafeHwnd(), rect.Width(), rect.Height() );

	GetRenderInterfaces()->GetRendererInterf()->SetDebugRenderer( (IDebugRenderer*)(GetDocument()->GetEntityList()) );

	return 0;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int COpenGLView::OnCreate( LPCREATESTRUCT lpcs )
{
	if(CView::OnCreate(lpcs) == -1)
		return -1;

	return 0;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COpenGLView::AttachView( IRenderView *pView )
{
	m_RenderView = pView;
	RECT rect;
	GetClientRect( &rect );
	pView->Init( GetDC()->GetSafeHdc(), rect.right-rect.left, rect.bottom-rect.top );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COpenGLView::OnMouseMove( UINT nFlags, CPoint point )
{
	
	GetDocument()->GetActiveTool()->OnMouseMove(this, nFlags, point);

	BaseClass::OnMouseMove(nFlags, point);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COpenGLView::OnLButtonDown( UINT nFlags, CPoint point )
{
	GetDocument()->GetActiveTool()->OnLButtonDown(this, nFlags, point);

	CView::OnLButtonDown(nFlags,point);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COpenGLView::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
	if( !m_RenderView )
		return;

	int pos = m_RenderView->HandleHScroll( nSBCode, (int)nPos );
	SetScrollPos(SB_HORZ, pos);
	DrawView();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COpenGLView::OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
	if( !m_RenderView )
		return;

	int pos = m_RenderView->HandleVScroll( nSBCode, (int)nPos );
	SetScrollPos(SB_VERT, pos);
	DrawView();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
BOOL COpenGLView::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
// 	if( !m_RenderView )
// 		return 0;
// 
// 	if( m_RenderView->HandleZoom( (int)zDelta ) )
// 		DrawView();
	GetDocument()->GetActiveTool()->OnWheelScroll(this, nFlags, zDelta, pt);
	return 0;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COpenGLView::DrawView( void )
{
	OnDraw( this->GetDC() );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COpenGLView::OnSize( UINT nType, int cx, int cy )
{
	if( !m_RenderView )
		return;
	m_pGLResources->SetActive( GetDC()->m_hDC );
	m_RenderView->Resize( cx, cy );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COpenGLView::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	GetDocument()->GetActiveTool()->OnKeyDown(this, nChar, nRepCnt, nFlags);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COpenGLView::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	GetDocument()->GetActiveTool()->OnKeyUp(this, nChar, nRepCnt, nFlags);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COpenGLView::OnLButtonUp(UINT nFlags, CPoint point)
{
	GetDocument()->GetActiveTool()->OnLButtonUp(this, nFlags, point);
	CView::OnLButtonUp(nFlags, point);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COpenGLView::OnMButtonDown(UINT nFlags, CPoint point)
{
	GetDocument()->GetActiveTool()->OnMButtonDown(this, nFlags, point);
	CView::OnMButtonDown(nFlags, point);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COpenGLView::OnMButtonUp(UINT nFlags, CPoint point)
{
	GetDocument()->GetActiveTool()->OnMButtonUp(this, nFlags, point);
	CView::OnMButtonUp(nFlags, point);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COpenGLView::OnRButtonDown(UINT nFlags, CPoint point)
{
	GetDocument()->GetActiveTool()->OnRButtonDown(this,nFlags,point);
	CView::OnRButtonDown(nFlags, point);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COpenGLView::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	GetDocument()->GetActiveTool()->OnRButtonDblClk(this, nFlags, point);
	CView::OnRButtonDblClk(nFlags, point);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COpenGLView::OnMButtonDblClk(UINT nFlags, CPoint point)
{
	GetDocument()->GetActiveTool()->OnMButtonDblClk(this, nFlags, point);
	CView::OnMButtonDblClk(nFlags, point);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COpenGLView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	GetDocument()->GetActiveTool()->OnLButtonDblClk(this, nFlags, point);
	CView::OnLButtonDblClk(nFlags, point);
}

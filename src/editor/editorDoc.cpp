// editor/editorDoc.cpp
// editor/editorDoc.cpp
// editor/editorDoc.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

// editorDoc.cpp : implementation of the CeditorDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "editor.h"
#endif

#include "editorDoc.h"
#include "CEditorEntityList.h"
#include <util/CScriptParser.h>
#include "ChildFrm.h"
#include "MainFrm.h"
#include "IEditorTool.h"
#include "CToolEditCam.h"
#include "CUndoRedo.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CeditorDoc

IMPLEMENT_DYNCREATE(CeditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CeditorDoc, CDocument)
	//ON_COMMAND(ID_FILE_SAVE, &CDocument::OnFileSave)
	//ON_COMMAND(ID_FILE_SAVE_AS, &CDocument::OnFileSaveAs)
	ON_COMMAND(ID_EDIT_UNDO, &CeditorDoc::OnEditUndo)
	ON_COMMAND(ID_EDIT_REDO, &CeditorDoc::OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, &CeditorDoc::OnUpdateEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, &CeditorDoc::OnUpdateEditUndo)
END_MESSAGE_MAP()


// CeditorDoc construction/destruction

CeditorDoc::CeditorDoc()
{
	// TODO: add one-time construction code here
	m_entityList = NULL;
	m_undoRedo = NULL;
}

CeditorDoc::~CeditorDoc()
{
	delete m_entityList;

	EditorToolList::iterator it = m_editorTools.begin();
	EditorToolList::iterator itEnd = m_editorTools.end();
	for( ; it != itEnd; it++ )
	{
		IEditorTool *pTool = *it;
		delete pTool;
	}
}

BOOL CeditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	//SetModifiedFlag();
	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	Init();

	CEditorEntity *pEnt = GetEntityList()->InsertNewEntity( "world" );

	SetModified();

	return TRUE;
}


void CeditorDoc::Init( void )
{
	m_undoRedo = new CUndoRedo();

	m_entityList = new CEditorEntityList();
	GetRenderInterfaces()->GetRendererInterf()->SetDebugRenderer( m_entityList );
	m_editorTools.push_back( new CToolEditCam() );
	m_activeTool = *m_editorTools.begin();

	for( EditorToolList::iterator it = m_editorTools.begin(); it != m_editorTools.end(); it++ )
		(*it)->Init();
}


// CeditorDoc serialization

void CeditorDoc::Serialize(CArchive& ar)
{
	std::wstring filename( ar.m_strFileName );
	bool bStore = ar.IsStoring() == TRUE;
	ar.Close();

	if( bStore )
	{
		std::ofstream os( filename );
		if( !os.good() )
		{
			error( "Could not save file, might be write protected or something." );
			os.close();
			return;
		}

		m_entityList->Serialize( os );

		if( !os.good() )
			error( "Error saving file, maybe out of space?" );
		
		os.close();
	}
	else
	{
		if( !m_entityList )
			m_entityList = new CEditorEntityList();

		std::ifstream is( filename );
		if( !is.good() )
		{
			error( "Could not read file" );
			is.close();
			return;
		}
		is.close();

		CScriptParser parser;
		if( !parser.ParseFile( CW2A(filename.c_str()) ) )
		{
			error( "Error parsing file." );
			return;
		}

		m_entityList->Deserialize(&parser);
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CeditorDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CeditorDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CeditorDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CeditorDoc diagnostics

#ifdef _DEBUG
void CeditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CeditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CeditorDoc commands


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CeditorDoc * GetActiveDocument( void )
{
	CChildFrame *chldFrm = (CChildFrame*)(((CMainFrame*)AfxGetMainWnd())->MDIGetActive());
	if( !chldFrm )
		return NULL;

	return (CeditorDoc*)chldFrm->GetActiveView()->GetDocument();
}


BOOL CeditorDoc::OnSaveDocument(LPCTSTR lpszPathName)
{

	std::ofstream os( lpszPathName );
	if( !os.good() )
	{
		error( "Could not save file, might be write protected or something." );
		os.close();
		return FALSE;
	}

	m_entityList->Serialize( os );

	if( !os.good() )
	{
		error( "Error saving file, maybe out of space?" );
		return FALSE;
	}

	os.close();

	SetModified(false);

	return TRUE;
	//return CDocument::OnSaveDocument(lpszPathName);
}


BOOL CeditorDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	//if (!CDocument::OnOpenDocument(lpszPathName))
		//return FALSE;

	Init();

	std::ifstream is( lpszPathName );
	if( !is.good() )
	{
		error( "Could not read file" );
		is.close();
		return FALSE;
	}
	is.close();

	CScriptParser parser;
	if( !parser.ParseFile( CW2A(lpszPathName) ) )
	{
		error( "Error parsing file." );
		return FALSE;
	}

	m_entityList->Deserialize(&parser);

	return TRUE;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CeditorDoc::SetActiveTool( tools_e tool )
{
	for( auto it = m_editorTools.begin(); it != m_editorTools.end(); it++ )
	{
		IEditorTool *pTool = *it;
		if( pTool->GetToolId() == tool )
		{
			m_activeTool = pTool;
			break;
		}
	}

	UpdatePropertiesWnd();
	//UpdateAllViews(NULL);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CeditorDoc::SelectSingleEntity( CEditorEntity *pEnt )
{
	m_selectedEnts.resize(1);
	m_selectedEnts[0] = pEnt;
	UpdatePropertiesWnd();
	//UpdateAllViews(NULL);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CeditorDoc::UnselectEntity( CEditorEntity *pEnt )
{
	for( size_t i = 0; i < m_selectedEnts.size(); i++ )
	{
		if( m_selectedEnts[i] == pEnt )
			m_selectedEnts.erase( m_selectedEnts.begin() + i );
	}
	UpdatePropertiesWnd();
	//UpdateAllViews(NULL);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CeditorDoc::IsSelected( CEditorEntity *pEnt )
{
	for( UINT i = 0; i < m_selectedEnts.size(); i++ )
		if( m_selectedEnts[i] == pEnt )
			return true;
	return false;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CeditorDoc::UnselectAllEntities( void )
{
	m_selectedEnts.clear();
	UpdatePropertiesWnd();
	//UpdateAllViews(NULL);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CEditorEntity* CeditorDoc::GetSelectedEntity( UINT i )
{
	if( i >= m_selectedEnts.size() )
		return NULL;
	else 
		return m_selectedEnts[i];
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CeditorDoc::DeleteSelectedEntity( void )
{
	CEditorEntityList *list = GetEntityList();

	for( UINT i = 0; i < m_selectedEnts.size(); i++ )
		list->RemoveEntity(m_selectedEnts[i], true);

	m_selectedEnts.clear();
	SetModified();
	UpdatePropertiesWnd();
	//UpdateAllViews(NULL);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CeditorDoc::SelectAdditionalEntity( CEditorEntity *pEnt )
{
	m_selectedEnts.push_back(pEnt);
	UpdatePropertiesWnd();
	//UpdateAllViews(NULL);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CeditorDoc::SetModified( bool mod /*= true*/ )
{
	SetModifiedFlag( mod ? TRUE : FALSE );
	UpdateFrameCounts();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CeditorDoc::UpdatePropertiesWnd()
{
	CPropertiesWnd *propWnd = theApp.GetMainFrame()->GetPropertiesWnd();
	if( propWnd )
		propWnd->UpdateEntitySelection(this);

	CInputOutputWnd *inOutWnd = theApp.GetMainFrame()->GetInputOutputWnd();
	if( inOutWnd )
		inOutWnd->UpdateEntitySelection(this);
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CeditorDoc::OnCloseDocument()
{
	UnselectAllEntities();
	UpdatePropertiesWnd();
	CDocument::OnCloseDocument();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CeditorDoc::OnEditUndo()
{
	m_undoRedo->Undo();
	UpdatePropertiesWnd();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CeditorDoc::OnUpdateEditUndo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_undoRedo->CanUndo() ? TRUE : FALSE);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CeditorDoc::OnEditRedo()
{
	m_undoRedo->Redo();
	UpdatePropertiesWnd();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CeditorDoc::OnUpdateEditRedo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_undoRedo->CanRedo() ? TRUE : FALSE);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CeditorDoc::DuplicateSelectedEntities( bool selectNewEnts /*= true */ )
{
	if(m_selectedEnts.empty())
		return;

	SelectedEntityList old = m_selectedEnts;
	UnselectAllEntities();
	for(const CEditorEntity *oldEnt : old)
	{
		CEditorEntity *newEnt = m_entityList->DuplicateEntity(oldEnt);
		newEnt->OnCreate();
		SelectAdditionalEntity(newEnt);
	}
}



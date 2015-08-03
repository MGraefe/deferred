// editor/editorDoc.h
// editor/editorDoc.h
// editor/editorDoc.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

// editorDoc.h : interface of the CeditorDoc class
//


#pragma once

class CEditorEntityList;
class CUndoRedo;
class CEditorEntity;
class IEditorTool;
class CUndoRedo;
typedef std::vector<CEditorEntity*> SelectedEntityList;
typedef std::vector<IEditorTool*> EditorToolList;
class CeditorDoc : public CDocument
{
protected: // create from serialization only
	CeditorDoc();
	DECLARE_DYNCREATE(CeditorDoc)
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS

public:
	void Init( void );
	virtual ~CeditorDoc();

	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	CEditorEntityList *GetEntityList(void) { return m_entityList; }
	void SetEntityList( CEditorEntityList *list ) { m_entityList = list; }
	void SelectSingleEntity( CEditorEntity *pEnt );
	void UnselectEntity( CEditorEntity *pEnt );
	bool IsSelected( CEditorEntity *pEnt );
	void UnselectAllEntities(void);
	void SelectAdditionalEntity( CEditorEntity *pEnt );

	template<class InputIterator>
	void SelectAdditionalEntities(InputIterator begin, InputIterator end)
	{
		m_selectedEnts.assign(begin, end);
		UpdatePropertiesWnd();
	}


	void DeleteSelectedEntity( void );
	void DuplicateSelectedEntities( bool selectNewEnts = true );
	const SelectedEntityList &GetSelectedEntityList( void ) { return m_selectedEnts; }
	CEditorEntity* GetSelectedEntity(UINT i);

	const EditorToolList& GetToolList(void) { return m_editorTools; }
	IEditorTool *GetActiveTool(void) { return m_activeTool; }
	void SetActiveTool( tools_e tool );

	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);

	void SetModified(bool mod = true);
	void UpdatePropertiesWnd(void);

private:
	CEditorEntityList *m_entityList;
	SelectedEntityList m_selectedEnts;
	EditorToolList m_editorTools;
	IEditorTool *m_activeTool;
	CUndoRedo *m_undoRedo;
public:
	virtual void OnCloseDocument();
	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditUndo(CCmdUI *pCmdUI);
};


CeditorDoc *GetActiveDocument(void);

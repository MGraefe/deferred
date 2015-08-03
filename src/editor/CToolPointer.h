// editor/CToolPointer.h
// editor/CToolPointer.h
// editor/CToolPointer.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


//CToolPointer.h

#pragma once
#ifndef deferred__editor__CToolPointer_H__
#define deferred__editor__CToolPointer_H__

#include "IEditorTool.h"

#include <vector>
class CToolTranslateEffector;
class CToolRotateEffector;
class CEditorEntity;
class CToolEffector;
typedef std::vector<CEditorEntity*> SelectedEntityList;


class CToolPointer : public IEditorTool
{

public:
	enum mode_e {
		TRANSLATE = 0,
		ROTATE,
	};

public:
	CToolPointer();
	virtual ~CToolPointer();

public:
	virtual void OnMouseMove( COpenGLView* view, UINT nFlags, CPoint point );
	virtual void OnLButtonDown( COpenGLView* view, UINT nFlags, CPoint point );
	virtual void OnRButtonDown( COpenGLView* view, UINT nFlags, CPoint point ) {}
	virtual void OnMButtonDown( COpenGLView* view, UINT nFlags, CPoint point ) {}
	virtual void OnKeyDown( COpenGLView* view, UINT nChar, UINT nRepCnt, UINT flags );
	virtual void OnLButtonUp( COpenGLView* view, UINT nFlags, CPoint point );
	virtual void OnRButtonUp( COpenGLView* view, UINT nFlags, CPoint point ) {}
	virtual void OnMButtonUp( COpenGLView* view, UINT nFlags, CPoint point ) {}
	virtual void OnKeyUp( COpenGLView* view, UINT nChar, UINT nRepCnt, UINT flags ) {}
	virtual void OnRButtonDblClk( COpenGLView* view, UINT nFlags, CPoint point ) {}
	virtual void OnMButtonDblClk( COpenGLView* view, UINT nFlags, CPoint point ) {}
	virtual void OnLButtonDblClk( COpenGLView* view, UINT nFlags, CPoint point ) {}
	virtual void OnWheelScroll( COpenGLView* view, UINT nFlags, short zDelta, CPoint pt ) {}
	virtual void OnVScroll( COpenGLView* view, UINT code, int pos ) {}
	virtual void OnHScroll( COpenGLView* view, UINT code, int pos ) {}

	virtual void Render3d( void );
	virtual void Init( void );

public:
	void HandleSelectionClick( UINT nFlags, CEditorEntity * pEnt );
	void StartDragMode( CToolEffector *effector, COpenGLView* view );

private:
	void HandleEffectorMovement( COpenGLView* view, CPoint &point );
	void HandleEffectorHighlighting( COpenGLView* view, CPoint &point );
	CToolEffector *GetEffectorUnderMouse( COpenGLView *view, CPoint mousepos );
	Vector3f GetEntityCenterPoint(const SelectedEntityList &ents);
	CEditorEntity *GetEntityUnderMouse(COpenGLView *view, const CPoint &point);
	CToolEffector **GetActiveEffectors( void );

private:
	//For modification
	CToolTranslateEffector* m_translateEffectors[3];
	CToolRotateEffector* m_rotateEffectors[3];
	bool m_dragMode;
	mode_e m_mode;
	CPoint m_lastMousePos;
	CPoint m_lastGlobalMousePos;
	COpenGLView *m_lastDragView;
	CToolEffector* m_selectedEffector;
};

#endif // deferred__editor__CToolPointer_H__

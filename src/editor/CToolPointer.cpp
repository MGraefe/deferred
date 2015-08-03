// editor/CToolPointer.cpp
// editor/CToolPointer.cpp
// editor/CToolPointer.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#include "stdafx.h"
#include "CToolPointer.h"
#include "COpenGLView.h"
#include "C3dView.h"
#include "CCamera.h"
#include "editorDoc.h"
#include "ChildFrm.h"
#include "CEditorEntityList.h"
#include "CEditorEntity.h"
#include "CToolTranslateEffector.h"
#include "CToolRotateEffector.h"
#include <util/CConVar.h>


CConVar editor_trace_debug("editor.trace.debug", "0");

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CToolPointer::CToolPointer() : 
	IEditorTool(TOOL_GENERIC)
{
	m_dragMode = false;
	m_mode = TRANSLATE;
	m_selectedEffector = NULL;
	m_lastDragView = NULL;


	for( int i = 0; i < 3; i++ )
	{
		Vector3f axis(vec3_null);
		axis[i] = 1.0f;

		m_translateEffectors[i] = new CToolTranslateEffector(axis);
		m_translateEffectors[i]->SetAxis(axis);
		m_rotateEffectors[i] = new CToolRotateEffector(axis);
		m_rotateEffectors[i]->SetAxis(axis);
	}
}

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CToolPointer::~CToolPointer()
{
	for( int i = 0; i < 3; i++ )
	{
		delete m_translateEffectors[i];
		delete m_rotateEffectors[i];
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolPointer::OnLButtonDown( COpenGLView* view, UINT nFlags, CPoint point )
{
	if( view->GetViewType() != VIEW_3D )
		return;

	m_lastMousePos = point;
	GetCursorPos( &m_lastGlobalMousePos );
	
	CToolEffector *effector = NULL;

	if( GetActiveDocument()->GetSelectedEntity(0) && 
		((m_mode == TRANSLATE && CToolEffector::IsAnySelectedEntityTranslatable()) ||
		 (m_mode == ROTATE && CToolEffector::IsAnySelectedEntityRotatable())) &&
		(effector = GetEffectorUnderMouse(view, point)) != NULL )
	{
		StartDragMode(effector, view);
	}
	else
	{
		HandleSelectionClick(nFlags, GetEntityUnderMouse(view, point));
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CEditorEntity *CToolPointer::GetEntityUnderMouse(COpenGLView *view, const CPoint &point)
{
	IRenderView *rview = view->GetRenderView();
	Vector3f raydir, rayorig;
	rview->CreateMouseRay( point, rayorig, raydir );
	CEditorEntity *pEnt = GetActiveDocument()->GetEntityList()->TraceLine( rayorig, raydir );
	return pEnt;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolPointer::OnLButtonUp( COpenGLView* view, UINT nFlags, CPoint point )
{
	if( m_selectedEffector && m_dragMode )
		m_selectedEffector->DragEnd(true);

	m_selectedEffector = NULL;
	m_lastDragView = NULL;
	m_dragMode = false;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolPointer::Render3d( void )
{	
	const SelectedEntityList &ents = GetActiveDocument()->GetSelectedEntityList();
	if( !CToolEffector::IsAnySelectedEntityTranslatable() )
		return;

	C3dView *p3dView = (C3dView*)GetActiveChildFrame()->GetView(VIEW_3D)->GetRenderView();
	CShader *pShd = GetShaderManager()->GetActiveShader();
	CCamera *pCam = p3dView->GetCamera();
	Vector3f vCamPos = pCam->GetAbsPos();
	Vector3f vEntPos = GetEntityCenterPoint(ents);
	float camDist = vCamPos.distTo(vEntPos);

	pShd->EnableTexturing(false);

	CToolEffector **effectors = GetActiveEffectors();

	for( int i = 0; i < 3; i++ )
	{
		effectors[i]->SetPosition(vEntPos);
		effectors[i]->UpdateSize(camDist, pCam->GetFov());
		effectors[i]->Render();
	}

	if( editor_trace_debug.GetBool() )
	{	
		CRay ray;
		p3dView->CreateMouseRay(m_lastMousePos, ray.orig, ray.dir);
		pShd->SetDrawColor(1.0f, 1.0f, 0.0f, 1.0f);
		draw_line(ray.orig, ray.orig + ray.dir * 5000.0f);
	}

	GetShaderManager()->GetActiveShader()->SetDrawColor(1.0f, 1.0f, 1.0f, 1.0f);
	pShd->EnableTexturing(true);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolPointer::Init( void )
{
	for( int i = 0; i < 3; i++ ) 
	{
		m_translateEffectors[i]->Init();
		m_rotateEffectors[i]->Init();
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolPointer::OnMouseMove( COpenGLView* view, UINT nFlags, CPoint point )
{
	//ConsoleMessage( "mouse movement to: %i, %i", point.x, point.y );
	if( !GetActiveDocument()->GetSelectedEntity(0) )
		return;

	if( m_dragMode && m_selectedEffector && m_lastDragView )
	{
		CPoint globalPoint;
		GetCursorPos( &globalPoint );
		m_lastDragView->ScreenToClient( &globalPoint );
		HandleEffectorMovement(m_lastDragView, globalPoint);
	}
	else if( view->GetViewType() == VIEW_3D )
		HandleEffectorHighlighting(view, point);
}


//---------------------------------------------------------------
// Purpose: Returns NULL if no effector is hit
//---------------------------------------------------------------
CToolEffector* CToolPointer::GetEffectorUnderMouse( COpenGLView *view, CPoint mousepos )
{
	IRenderView *renderView = view->GetRenderView();

	Vector3f rayorig, raydir;
	renderView->CreateMouseRay( mousepos, rayorig, raydir );

	float hitDist = FLOAT_MAX;

	tracelineinfo_t info;
	info.start = rayorig;
	info.dir = raydir;
	info.distMax = FLOAT_MAX;

	CToolEffector *effector = NULL;
	CToolEffector **effectors = GetActiveEffectors();

	for( int i = 0; i < 3; i++ )
	{
		if( effectors[i]->TraceLine(info) && info.distHit < info.distMax )
		{
			effector = effectors[i];
			info.distMax = info.distHit;
		}
	}

	return effector;
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolPointer::HandleEffectorHighlighting( COpenGLView* view, CPoint &point )
{
	IRenderView *pView = view->GetRenderView();
	if( point.x < 0 || point.y < 0 || point.x > pView->GetWidth() || point.y > pView->GetHeight() )
		return;

	CToolEffector *effector = GetEffectorUnderMouse( view, point );
	CToolEffector **effectors = GetActiveEffectors();

	bool needUpdate = false;
	for( int i = 0; i < 3; i++ )
	{
		bool newstate = effectors[i] == effector;
		bool oldstate = effectors[i]->SetSelected(newstate);
		if( newstate != oldstate )
			needUpdate = true;
	}

	if( editor_trace_debug.GetBool() )
		m_lastMousePos = point;

	//if( needUpdate || editor_trace_debug.GetBool() )
	//	GetActiveDocument()->UpdateAllViews(NULL);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolPointer::HandleEffectorMovement( COpenGLView* view, CPoint &point )
{
	//Get Mouse movement dir
	Vector2f move2d( point.x - m_lastMousePos.x, -point.y + m_lastMousePos.y );
	if( move2d.x != 0.0f && move2d.y != 0.0f )
	{
		C3dView *renderView = (C3dView*)view->GetRenderView();
		CCamera *pCam = renderView->GetCamera();
		const Vector3f &camPos = pCam->GetAbsPos();
		float camDist = m_selectedEffector->GetPosition().distTo(camPos);
		float fov = renderView->GetCamera()->GetFov();

		CRay clickRay;
		renderView->CreateMouseRay(point, clickRay.orig, clickRay.dir);
		m_selectedEffector->DragUpdate(clickRay, true);

		for( int i = 0; i < 3; i++ )
		{
			CToolEffector *effector = GetActiveEffectors()[i];
			effector->SetPosition(m_selectedEffector->GetPosition());

			if(effector != m_selectedEffector)
			{
				Vector3f newAxis = m_selectedEffector->GetRotation() * effector->GetDragStartAxis();
				effector->SetAxis(newAxis);
			}
		}

		//GetActiveDocument()->UpdateAllViews(NULL);
	}

	////Get Mouse movement dir
	//Vector2d move2d( point.x - m_lastMousePos.x, -point.y + m_lastMousePos.y );
	////m_lastMousePos = point;
	//if( move2d.x != 0.0f && move2d.y != 0.0f )
	//{
	//	//Get the camera distance
	//	C3dView *renderView = (C3dView*)view->GetRenderView();
	//	CCamera *pCam = renderView->GetCamera();
	//	const Vector3f &camPos = pCam->GetAbsPos();
	//	float camDist = m_lastEffectorPos.distTo( camPos );

	//	//Get the real 2d movement
	//	Vector2d realMove = m_effectorDirProjected * move2d.Dot(m_effectorDirProjected);
	//	float dir = sign(m_effectorDirProjected.Dot(move2d)); //movement direction

	//	float fov = renderView->GetCamera()->GetFov();
	//	float moveDist = realMove.length() * camDist * TAN(fov*0.5f) / (float)renderView->GetHeight();

	//	//Calculate new entity centers
	//	Vector3f newEffectorPos = m_lastEffectorPos + m_selectedTranslateEffector->GetAxisVector() * moveDist * dir * 2.0f;
	//	Vector3f effectorMove3d = newEffectorPos - m_lastEffectorPos;

	//	//Get Camera direction vector
	//	Vector3f camDir = pCam->GetAbsAngles() * Vector3f(0.0f, 0.0f, -1.0f);
	//	Vector3f entDir = (newEffectorPos - camPos).Normalize();

	//	//When dragging the entity to far outside the window, our calculations
	//	//result in infinite values for the movement. Do this quick check
	//	//to prevent this.
	//	float fDot = camDir.Dot(entDir);
	//	if( camDir.Dot(entDir) > COS(75.0f) )
	//	{
	//		const SelectedEntityList &ents = GetActiveDocument()->GetSelectedEntityList();
	//		for( UINT i = 0; i < ents.size(); i++ )
	//			if( ents[i]->IsTranslatable() )
	//				ents[i]->SetAbsCenter( m_oldEntPositions[i] + effectorMove3d );

	//		GetActiveDocument()->UpdateAllViews(NULL);
	//	}
	//}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
Vector3f CToolPointer::GetEntityCenterPoint( const SelectedEntityList &ents )
{
	return CEditorEntity::GetEntitiesCenter(ents.begin(), ents.end());
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolPointer::StartDragMode( CToolEffector *effector, COpenGLView* view )
{
	//Start drag-mode!
	m_dragMode = true;
	m_lastDragView = view;

	CRay clickRay;
	view->GetRenderView()->CreateMouseRay(m_lastMousePos, clickRay.orig, clickRay.dir);
	m_selectedEffector = effector;

	CToolEffector **effectors = GetActiveEffectors();

	for( int i = 0; i < 3; i++ )
		effectors[i]->DragStart(clickRay, effectors[i] == m_selectedEffector);

	////set the initial entity positions
	//m_lastEffectorPos = GetEntityCenterPoint( GetActiveDocument()->GetSelectedEntityList() );
	//const SelectedEntityList &ents = GetActiveDocument()->GetSelectedEntityList();
	//m_oldEntPositions.resize(ents.size());
	//for( UINT i = 0; i < ents.size(); i++ )
	//	m_oldEntPositions[i] = ents[i]->GetAbsCenter();

	//CCamera *pCam = ((C3dView*)view->GetRenderView())->GetCamera();
	//const Matrix4f &modelViewMat = pCam->GetViewSpaceMat();
	//const Matrix4f &projMat = pCam->GetProjMat();
	//m_effectorDirProjected = m_selectedTranslateEffector->GetProjected2dSize( projMat * modelViewMat ).Normalize();
	////ConsoleMessage( "effectorDir = (%f %f)", m_effectorDirProjected.x, m_effectorDirProjected.y );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolPointer::HandleSelectionClick( UINT nFlags, CEditorEntity * pEnt )
{
	if( nFlags == (MK_CONTROL | MK_LBUTTON) )
	{
		//When CTRL is pressed we want to add the hit entity to our selection. Also when CTRL is pressed and we dont
		//hit an entity nothing happens.
		if( pEnt )
		{
			if( GetActiveDocument()->IsSelected(pEnt) )
				GetActiveDocument()->UnselectEntity(pEnt);
			else
				GetActiveDocument()->SelectAdditionalEntity(pEnt);
		}
	}
	else
	{
		//When CTRL is NOT pressed we just set the hit entity as our selected one.
		//Also when CTRL is NOT pressed and we click in the void we un-select all entities.
		if( pEnt )
			GetActiveDocument()->SelectSingleEntity(pEnt);
		else
			GetActiveDocument()->UnselectAllEntities();
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolPointer::OnKeyDown( COpenGLView* view, UINT nChar, UINT nRepCnt, UINT flags )
{
	if( flags & 0x2000 ) //Check for ALT on 13th bit
		return;
	if( GetKeyState(VK_LCONTROL) & 0x8000 || GetKeyState(VK_RCONTROL) & 0x8000 )
		return;
	if( GetKeyState(VK_SHIFT) & 0x8000 )
		return;

	mode_e oldMode = m_mode;

	if( nChar == 'V' )
		m_mode = ROTATE;
	if( nChar == 'C' )
		m_mode = TRANSLATE;

	//if( oldMode != m_mode )
	//	GetActiveDocument()->UpdateAllViews(NULL);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CToolEffector ** CToolPointer::GetActiveEffectors( void )
{
	switch(m_mode)
	{
	case TRANSLATE:
		return (CToolEffector**)m_translateEffectors;
	case ROTATE:
		return (CToolEffector**)m_rotateEffectors;
	}

	return NULL;
}


// editor/CToolEffector.h
// editor/CToolEffector.h
// editor/CToolEffector.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
//CToolEffector.h

#pragma once
#ifndef deferred__editor__CToolEffector_H__
#define deferred__editor__CToolEffector_H__

#include <util/maths.h>
#include "tracelineinfo.h"

class CToolEffector
{
public:
	CToolEffector( const Vector3f &color );
	~CToolEffector();

	virtual void Init( void ) = 0;
	virtual void Render( void ) = 0;
	virtual bool TraceLine( tracelineinfo_t &info ) = 0;
	virtual void SetPosition( const Vector3f &pos ) = 0;
	virtual const Vector3f &GetPosition( void ) = 0;
	virtual void UpdateSize( float cameraDistance, float camFov ) = 0;


	virtual void DragStart(const CRay &clickRay, bool active) = 0;
	virtual void DragUpdate(const CRay &clickRay, bool active) = 0;
	virtual void DragEnd(bool active) = 0;

	virtual Quaternion GetRotation( void ) { return Quaternion(); }
	virtual Vector3f GetDragStartAxis( void ) { return m_axis; }
	virtual bool SetSelected( bool selected = true );
	virtual void SetAxis( const Vector3f &axis );
	virtual const Vector3f &GetAxis( void );

	static bool IsAnySelectedEntityTranslatable( void );
	static bool IsAnySelectedEntityRotatable( void );
	static bool IsAllSelectedEntitiesTranslatable( void );
	static bool IsAllSelectedEntitiesRotatable( void );
protected:
	Vector3f m_baseColor;
	bool m_selected;
	Vector3f m_axis;
};




#endif // deferred__editor__CToolEffector_H__

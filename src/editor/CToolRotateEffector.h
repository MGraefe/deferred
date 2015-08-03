// editor/CToolRotateEffector.h
// editor/CToolRotateEffector.h
// editor/CToolRotateEffector.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
//CToolRotateEffector.h

#pragma once
#ifndef deferred__editor__CToolRotateEffector_H__
#define deferred__editor__CToolRotateEffector_H__

#include <vector>
#include "tracelineinfo.h"
#include "CToolEffector.h"
class CeditorDoc;

class CToolRotateEffector : public CToolEffector
{
public:
	CToolRotateEffector( const Vector3f &color ); 
	~CToolRotateEffector();

	void Init( void );
	void Render( void );
	bool TraceLine( tracelineinfo_t &info );
	void SetPosition( const Vector3f &pos );
	const Vector3f &GetPosition( void );
	void UpdateSize( float cameraDistance, float camFov );
	Quaternion GetLocalRotation();

	Vector3f FindNearestPoint(const Vector3f &rayorig, const Vector3f &raydir );
	float GetClickedRotation(const Vector3f &startPointUntransformed, const CRay &clickRay);
	Vector3f GetRotateAxis(void);

	Quaternion GetRotation( void ) { return m_rotation; }
	Vector3f GetDragStartAxis( void ) { return m_dragStartAxis; }

	void DragStart(const CRay &clickRay, bool active);
	void DragUpdate(const CRay &clickRay, bool active);
	void DragEnd(bool active);

private:
	static const int circle_segments = 64;

private:
	float m_scale;
	Vector3f m_position;
	Vector3f m_axis_circle[circle_segments];
	Vector3f m_dragStartUntransformed;
	std::vector<Vector3f> m_entityStartPositions;
	std::vector<Angle3d> m_entityStartRotations;
	Quaternion m_rotation;
	Vector3f m_dragStartAxis;
	bool m_dragging;
};

#endif // deferred__editor__CToolRotateEffector_H__

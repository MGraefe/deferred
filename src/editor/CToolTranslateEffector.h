// editor/CToolTranslateEffector.h
// editor/CToolTranslateEffector.h
// editor/CToolTranslateEffector.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
//CToolTranslateEffector.h

#pragma once
#ifndef deferred__editor__CToolTranslateEffector_H__
#define deferred__editor__CToolTranslateEffector_H__

#include "CToolEffector.h"
#include "tracelineinfo.h"
class CeditorDoc;

class CToolTranslateEffector : public CToolEffector
{
public:
	CToolTranslateEffector( const Vector3f &color );
	~CToolTranslateEffector();
	void Init( void );
	void Render( void );
	bool TraceLine( tracelineinfo_t &info );
	void SetPosition( const Vector3f &pos );
	const Vector3f &GetPosition( void );
	void UpdateSize( float cameraDistance, float camFov );

	void DragStart(const CRay &clickRay, bool active);
	void DragUpdate(const CRay &clickRay, bool active);
	void DragEnd(bool active);

	void SetAxis( const Vector3f &axis );

	Quaternion GetLocalRotation();
private:
	float m_scale;
	Vector3f m_modelSize;
	Vector3f m_position;
	Vector3f m_clickStart;
	Vector3f m_startPos;
	std::vector<Vector3f> m_entityStartPos;
	CModel *m_arrowModel;
};

#endif // deferred__editor__CToolTranslateEffector_H__

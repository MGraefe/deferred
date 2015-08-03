// server/CViewTracer.h
// server/CViewTracer.h
// server/CViewTracer.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__server__CViewTracer_H__
#define deferred__server__CViewTracer_H__

#include "CBaseEntity.h"
#include "CCamera.h"
#include "rpm_file.h"
#include "CModel.h"


class CViewTracer: public CBaseEntity
{
public:
	CViewTracer::CViewTracer();
	void Init( CCamera *pCam, rpm_file_t *pWorld );
	void Render( void );

private:
	CCamera *m_pCam;
	rpm_file_t *m_pWorld;
	CModel *m_pSphere;
};



#endif

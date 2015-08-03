// editor/CEntityTypeBlock.h
// editor/CEntityTypeBlock.h
// editor/CEntityTypeBlock.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__editor__CEntityTypeBlock_H__
#define deferred__editor__CEntityTypeBlock_H__

#include "CEditorEntity.h"

//Forward declarations
//template<typename T>
//class CVertexIndexBuffer;
//class CVertexBuffer;

class CEntityTypeBlock : public CEditorEntity
{
	DECLARE_EDITORENT(CEditorEntity, CEntityTypeBlock)
	DECLARE_BASECLASS(CEditorEntity)

public:
	CEntityTypeBlock( const CScriptSubGroup *pGrp, int index );
	CEntityTypeBlock( const std::string &classname, int index );
	CEntityTypeBlock( const CEntityTypeBlock &other, int index );

public:
	virtual void OnCreate( void );
	virtual void OnDestroy( void );
	virtual void OnParamChanged( const std::string &key, const std::string &oldval, const std::string &newval );
	virtual bool TraceLine( tracelineinfo_t &info );
	virtual void Render( void );

private:
	void updateInternals();
	void loadVertices();
	void destroyVertices();
	void sizeVertices(const Vector3f &size);
	void calcUvs( rendervert_t * buf, int numVerts );
	void resize();
	void loadMaterial();
	AABoundingBox getAABB();
private:
	CVertexBuffer *m_vertBuffer;
	CVertexIndexBuffer<USHORT> *m_vertIndexBuffer;
	int m_materialId;
	float m_texScale;
	Vector3f m_size;
};

#endif // deferred__editor__CEntityTypeBlock_H__
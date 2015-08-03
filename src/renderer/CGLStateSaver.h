// renderer/CGLStateSaver.h
// renderer/CGLStateSaver.h
// renderer/CGLStateSaver.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__renderer__CGLStateSaver_H__
#define deferred__renderer__CGLStateSaver_H__

#include <map>
#include <util/basic_types.h>
#include "CRenderInterf.h"

PUSH_PRAGMA_DISABLE_DLL_MEMBER_WARNING;

typedef std::map<UINT,bool> StateMap;

class DLL CGLStateSaver
{
public:
	CGLStateSaver();
	void Enable( UINT state );
	void Disable( UINT state );
	void EnableClientState( UINT state );
	void DisableClientState( UINT state );
	void EnableVertexAttribArray( UINT attrib );
	void ActiveTexture( UINT tex );
	void DepthFunc( UINT func );
	void CullFace( UINT face );

	void DisableVertexAttribArray( UINT attrib, bool bDisableLastUsed = false );
	bool GetState( const UINT &state );
	void ClearStates( void );

private:
	StateMap m_ServerStates;
	StateMap m_VertexAttribStates;
	UINT m_iLastVertexAttrib;
	UINT m_iLastActiveTexture;
	UINT m_iLastDepthFunc;
	UINT m_iLastCullFace;
};


inline CGLStateSaver *GetGLStateSaver( void )
{
	return g_RenderInterf->GetGLStateSaver();
}

POP_PRAGMA_WARNINGS;

#endif

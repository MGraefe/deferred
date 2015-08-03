// client/CAppInterfaces.h
// client/CAppInterfaces.h
// client/CAppInterfaces.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__client__CAppInterfaces_H__
#define deferred__client__CAppInterfaces_H__

#include <util/IInterfaces.h>

//Forward declarations of interfaces
//We do this because we dont want do include every interface
class CButtonInputMgr;

class CAppInterfaces : public IInterfaces
{
public:
	CAppInterfaces();
	~CAppInterfaces();

	void Init( void );
	void Cleanup( void );

	DECL_INTERFACE( CButtonInputMgr, ButtonInputMgr );

public:
	void SetWnd( void* hWnd ) { m_hWnd = hWnd; }
	void* GetWnd( void ) { return m_hWnd; }

	void SetDevCtx( void* devCtx ) { m_DevCtx = devCtx; }
	void* GetDevCtx( void ) { return m_DevCtx; }


private:
	void* m_hWnd;
	void* m_DevCtx;
};

extern CAppInterfaces *const g_AppInterf;

#endif

// renderer/IRenderInterfaceAccessor.h
// renderer/IRenderInterfaceAccessor.h
// renderer/IRenderInterfaceAccessor.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
//IRenderInterfaceAccessor.h

#pragma once
#ifndef deferred__renderer__IRenderInterfaceAccessor_H__
#define deferred__renderer__IRenderInterfaceAccessor_H__

class IRenderInterfaceAccessor
{
public:
	IRenderInterfaceAccessor( CRenderInterfaces *pRenderInterfaces ) {
		m_RenderInterf = pRenderInterfaces;
	}

	//void SetRenderInterf( CRenderInterfaces *pInterf ) {
	//	m_RenderInterf = pInterf;
	//}

	CRenderInterfaces *GetRenderInterf( void ) {
		return m_RenderInterf;
	}

public:
	CRenderInterfaces* const m_RenderInterf;
};

#endif // deferred__renderer__IRenderInterfaceAccessor_H__

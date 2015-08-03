// server/CStaticModel.h
// server/CStaticModel.h
// server/CStaticModel.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__server__CStaticModel_H__
#define deferred__server__CStaticModel_H__

#include "CBaseEntity.h"

class CStaticModel : public CBaseEntity
{
	DECLARE_DATADESC_EDITOR(CBaseEntity, CStaticModel)
	DECLARE_ENTITY_LINKED()
public:
	void SetFilename( const char *pFileName );
	const std::string &GetFilename() const { return m_filename; }
private:
	NetVar( std::string, m_filename );
};

#endif
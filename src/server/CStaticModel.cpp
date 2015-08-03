// server/CStaticModel.cpp
// server/CStaticModel.cpp
// server/CStaticModel.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "CStaticModel.h"

LINK_ENTITY_TO_CLASSNAME_SERVER(CStaticModel, prop_dynamic)
BEGIN_DATADESC_EDITOR(CStaticModel)
	FIELD_STRING("modelname", m_filename);
	NETTABLE_ENTRY_PROXY(m_filename, CNetVarWriteProxyString)
END_DATADESC_EDITOR()

void CStaticModel::SetFilename( const char *pFileName )
{
	m_filename = pFileName;
}


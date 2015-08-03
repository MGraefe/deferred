// client/prop_models.h
// client/prop_models.h
// client/prop_models.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__client__prop_models_H__
#define deferred__client__prop_models_H__

#include <renderer/CModel.h>

class CClientPropPhysic: public CModel
{
	DECLARE_DATADESC_EDITOR(CModel, CClientPropPhysic)
	DECLARE_ENTITY_LINKED()
public: 
	CClientPropPhysic( int index = -1 ) : CModel(index) {};
};


class CClientPropDynamic: public CModel
{
	DECLARE_DATADESC_EDITOR(CModel, CClientPropDynamic)
	DECLARE_ENTITY_LINKED()
public: 
	CClientPropDynamic( int index = -1 ) : CModel(index) {};
};

#endif
// server/ServerRPM.h
// server/ServerRPM.h
// server/ServerRPM.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__server__ServerRPM_H__
#define deferred__server__ServerRPM_H__

#include <util/basic_types.h>

rendertri_t *LoadRPMFileServer( const char *filename, int &iTris );
//rendertri_t *LoadRPMFileServer( const char *filename, int &iTris, int **pMaterials = NULL, int *pNumOfMaterials = NULL );

#endif

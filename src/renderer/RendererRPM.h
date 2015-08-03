// renderer/RendererRPM.h
// renderer/RendererRPM.h
// renderer/RendererRPM.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


//RendererRPM.h

#pragma once
#ifndef deferred__renderer__RendererRPM_H__
#define deferred__renderer__RendererRPM_H__

#include <util/basic_types.h>
#include <util/rpm_file.h>

rendertri_t *LoadRPMFileRenderer( const char *filename, int &iTris, int **pMaterials = NULL, int *pNumOfMaterials = NULL, rpm_file_t *rpm = NULL );

#endif // deferred__renderer__RendererRPM_H__

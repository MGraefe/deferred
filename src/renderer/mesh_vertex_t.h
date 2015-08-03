// renderer/mesh_vertex_t.h
// renderer/mesh_vertex_t.h
// renderer/mesh_vertex_t.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__renderer__mesh_vertex_t_H__
#define deferred__renderer__mesh_vertex_t_H__

//#define MESH_VERTEX_T_SIZE 56
#define MESH_VERTEX_T_SIZE 32

struct mesh_vertex_t
{
	float uv[2];
	float normal[3];
	float position[3];
	//float tangent[3];
	//float bitangent[3];
};


#endif

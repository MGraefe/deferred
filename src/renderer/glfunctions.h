// renderer/glfunctions.h
// renderer/glfunctions.h
// renderer/glfunctions.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
#pragma once
#ifndef deferred__renderer__glfunctions_H__
#define deferred__renderer__glfunctions_H__

#include <util/maths.h>
#include <util/basic_types.h>
#include <vector>
#include <util/dll.h>
#include "vertexbuffer_formats.h"

struct rpm_triangle_t;
/*extern*/ inline void trans_rot( const Vector3f &vPos, const Angle3d &aRot );
DLL void trans_rot_model_start( const Vector3f &vPos, const Angle3d &aRot );
DLL void trans_rot_model_end( void );
DLL void trans_rot_model_start( const Vector3f &vPos, const Quaternion &quat );
///*extern*/ DLIST create_displaylist( rendertri_t *pTris, int iTris, int iShdTangentLocation, bool bShadowList,
//									bool bSortTris );


class CShader;
//DLL void init_vertexbuffer_vars( vertexbuffer_format_e format, const void *buffer = NULL );
//DLL void init_vertexbuffer_vars_rendervert_t( CShader *pShd, const void *buffer = NULL );
//DLL void init_vertexbuffer_vars_pos_normal_uvcoord( const void *buffer );
//DLL void init_vertexbuffer_vars_pos_uvcoord( const void *buffer );
//DLL void init_vertexbuffer_vars_uvcoord_pos( const void *buffer );
//DLL void init_vertexbuffer_vars_pos( const void *buffer );

DLL void rglPushClientAttrib(GLbitfield mask);
DLL void rglPopClientAttrib();



extern void split_trianglelist_translucency( const rendertri_t *trisIn,
				int iTrisIn,
				std::vector<int> &triIndexesNormal,
				std::vector<int> &triIndexesTranslucent );

DLL void draw_quad_textured( float x1, float y1, float x2, float y2, float z );
DLL void draw_quad_no_texture( float x1, float y1, float x2, float y2, float z );
DLL void draw_fullscreen_quad_no_tex( void );
DLL void draw_fullscreen_quad( void );
DLL void draw_quad_textured_normal( float x1, float y1, float x2, float y2, float z );

DLL void set_draw_color( float r, float g, float b, float a );

DLL void draw_line( const Vector3f &start, const Vector3f &end );
DLL void draw_line_strip( Vector3f *pVerts, int iVerts );
DLL void draw_lines( Vector3f *pVerts, UINT iVerts );
DLL void draw_line_cube( const Vector3f &center, float size );
DLL void draw_line_cube( const Vector3f &center, const Vector3f &size );


//note that this function is very slow, and for large quantities of sprites
//you should use draw_point_sprites
DLL void draw_point_sprite( const Vector3f &center, float size );

//note that you should use vertex buffers for really large quantities of sprites
DLL void draw_point_sprites( const Vector3f *positions, int count, float size );


#endif

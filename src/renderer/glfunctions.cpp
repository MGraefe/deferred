// renderer/glfunctions.cpp
// renderer/glfunctions.cpp
// renderer/glfunctions.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"


#include "glheaders.h"
#include <util/rpm_file.h>
#include <util/basic_types.h>
#include "CRenderInterf.h"
#include "TextureManager.h"
#include "CShaders.h"
#include <util/basicfuncs.h>


/*inline*/ void trans_rot( const Vector3f &vPos, const Angle3d &aRot )
{
	GetMVStack().Translate( vPos );
	GetMVStack().Rotate( aRot );
}

void trans_rot_model_start( const Vector3f &vPos, const Angle3d &aRot )
{
	GetMVStack().PushMatrix();
	trans_rot( vPos, aRot );
}

void trans_rot_model_start( const Vector3f &vPos, const Quaternion &quat )
{
	float mtx[16];
	quat.GetOpenglMatrix4x4( vPos, mtx );

	GetMVStack().PushMatrix();
	GetMVStack().MultMatrix( mtx );
}

void trans_rot_model_end( void )
{
	GetMVStack().PopMatrix();
}


void split_trianglelist_translucency( const rendertri_t *trisIn,
	int iTrisIn,
	std::vector<int> &triIndexesNormal,
	std::vector<int> &triIndexesTranslucent )
{
	CTextureManager *pMgr = g_RenderInterf->GetTextureManager();

	for( int i = 0; i < iTrisIn; i++ )
	{
		if( pMgr->IsMaterialTranslucent( trisIn[i].iTexture ) )
			triIndexesTranslucent.push_back(i);
		else
			triIndexesNormal.push_back(i);
	}
}


/*inline*/ void draw_quad_textured( float x1, float y1, float x2, float y2, float z )
{
	float vertices[] = {x1,	y1,	z, 0.0f, 0.0f,
						x2,	y1,	z, 1.0f, 0.0f,
						x2,	y2,	z, 1.0f, 1.0f,
						x1,	y2,	z, 0.0f, 1.0f };
	rglPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );
	GetShaderManager()->InitVertexbufferVars(VBF_POS_UV, vertices);
	GetShaderManager()->GetActiveShader()->UpdateMatrices();
	glDrawArrays( GL_QUADS, 0, 4 );
	rglPopClientAttrib();
}


/*inline*/ void draw_quad_textured_normal( float x1, float y1, float x2, float y2, float z )
{	
						//position        normal              tangent            uv
	float vertices[] = {x1,	y1,	z,	 0.0f, 0.0f, 1.0f,	 0.0f, 1.0f, 0.0f,	 0.0f, 0.0f,
						x2,	y1,	z,	 0.0f, 0.0f, 1.0f,	 0.0f, 1.0f, 0.0f,	 1.0f, 0.0f,
						x2,	y2,	z,	 0.0f, 0.0f, 1.0f,	 0.0f, 1.0f, 0.0f,	 1.0f, 1.0f,
						x1,	y2,	z,	 0.0f, 0.0f, 1.0f,	 0.0f, 1.0f, 0.0f,	 0.0f, 1.0f };
	rglPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );
	GetShaderManager()->InitVertexbufferVars( VBF_RENDERVERT, vertices );
	GetShaderManager()->GetActiveShader()->UpdateMatrices();
	glDrawArrays( GL_QUADS, 0, 4 );
	rglPopClientAttrib();
}


/*inline*/ void draw_quad_no_texture( float x1, float y1, float x2, float y2, float z )
{
	float vertices[] = {x1,	y1,	z,
						x2,	y1,	z,
						x2,	y2,	z,
						x1,	y2,	z };
	rglPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );
	GetShaderManager()->InitVertexbufferVars(VBF_POS, vertices);
	GetShaderManager()->GetActiveShader()->UpdateMatrices();
	glDrawArrays( GL_QUADS, 0, 4 );
	rglPopClientAttrib();
}



/*inline*/ void draw_triangle_no_texture( const Vector3f &v1, const Vector3f &v2, const Vector3f &v3 )
{
	float vertices[] = {v1.x, v1.y, v1.z, 
						v2.x, v2.y, v2.z, 
						v3.x, v3.y, v3.z };
	rglPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );
	GetShaderManager()->InitVertexbufferVars(VBF_POS, vertices);
	GetShaderManager()->GetActiveShader()->UpdateMatrices();
	glDrawArrays( GL_TRIANGLES, 0, 3 );
	rglPopClientAttrib();
}


void draw_fullscreen_quad_no_tex( void )
{
	GetMVStack().PushMatrix();
		GetMVStack().SetIdentity();
		GetProjStack().PushMatrix();
			GetProjStack().SetIdentity();
			draw_quad_no_texture( -1.0f, -1.0f, 1.0f, 1.0f, -1.0f );
		GetProjStack().PopMatrix();
	GetMVStack().PopMatrix();
}


void draw_fullscreen_quad( void )
{
	GetMVStack().PushMatrix();
		GetMVStack().SetIdentity();
		GetProjStack().PushMatrix();
			GetProjStack().SetIdentity();
			draw_quad_textured( -1.0f, -1.0f, 1.0f, 1.0f, -1.0f );
		GetProjStack().PopMatrix();
	GetMVStack().PopMatrix();
}



void set_draw_color( float r, float g, float b, float a )
{
	GetShaderManager()->GetActiveShader()->SetDrawColor(r,g,b,a);
}


void draw_line( const Vector3f &start, const Vector3f &end )
{
	float vertices[] = { start.x, start.y, start.z, end.x, end.y, end.z };
	rglPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );
	GetShaderManager()->InitVertexbufferVars(VBF_POS, vertices);
	GetShaderManager()->GetActiveShader()->UpdateMatrices();
	glDrawArrays( GL_LINES, 0, 2 );
	rglPopClientAttrib();
}

void draw_line_strip( Vector3f *pVerts, int iVerts )
{
	rglPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );
	GetShaderManager()->InitVertexbufferVars(VBF_POS, pVerts);
	GetShaderManager()->GetActiveShader()->UpdateMatrices();
	glDrawArrays( GL_LINE_STRIP, 0, iVerts );
	rglPopClientAttrib();
}

void draw_lines( Vector3f *pVerts, UINT iVerts )
{
	Assert( iVerts % 2 == 0 );
	rglPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );
	GetShaderManager()->InitVertexbufferVars(VBF_POS, pVerts);
	GetShaderManager()->GetActiveShader()->UpdateMatrices();
	glDrawArrays( GL_LINES, 0, iVerts );
	rglPopClientAttrib();
}

const float g_cubelineverts[] = {	-1, -1, -1, /*0, 0,*/		-1, -1,  1, /*0, 0,*/ 
									-1, -1,  1, /*0, 0,*/		-1,  1,  1, /*0, 0,*/ //Left quad
									-1,  1,  1, /*0, 0,*/		-1,  1, -1, /*0, 0,*/
									-1,  1, -1, /*0, 0,*/		-1, -1, -1, /*0, 0,*/

									 1, -1, -1, /*0, 0,*/		 1, -1,  1, /*0, 0,*/
									 1, -1,  1, /*0, 0,*/		 1,  1,  1, /*0, 0,*/ //Right quad
									 1,  1,  1, /*0, 0,*/		 1,  1, -1, /*0, 0,*/
									 1,  1, -1, /*0, 0,*/		 1, -1, -1, /*0, 0,*/

									-1, -1, -1, /*0, 0,*/		 1, -1, -1, /*0, 0,*/
									-1, -1,  1, /*0, 0,*/		 1, -1,  1, /*0, 0,*/ //Connection lines
									-1,  1, -1, /*0, 0,*/		 1,  1, -1, /*0, 0,*/
									-1,  1,  1, /*0, 0,*/		 1,  1,  1/*, 0, 0*/ };

void draw_line_cube( const Vector3f &center, float size )
{
	draw_line_cube(center, Vector3f(size,size,size));
}


void draw_line_cube( const Vector3f &center, const Vector3f &size )
{
	Vector3f sh = size*0.5f;
	GetMVStack().PushMatrix();
	GetMVStack().Translate(center);
	GetMVStack().Scale(sh.x, sh.y, sh.z);

	draw_lines( (Vector3f*)g_cubelineverts, 24 );

	GetMVStack().PopMatrix();
}

void rglPushClientAttrib(GLbitfield mask)
{

}

void rglPopClientAttrib()
{

}

//note that you should use vertex buffers for really large quantities of sprites
void draw_point_sprites( const Vector3f *positions, int count, float size )
{
	CShader *pShd = GetShaderManager()->GetActiveShader();
	pShd->SetPointSize( size );

	rglPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );
	GetShaderManager()->InitVertexbufferVars(VBF_POS, positions);
	pShd->UpdateMatrices();
	glDrawArrays( GL_POINTS, 0, count );
	rglPopClientAttrib();
}


//note that this function is very slow, and for large quantities of sprites
//you should use draw_point_sprites
void draw_point_sprite( const Vector3f &center, float size )
{
	draw_point_sprites( &center, 1, size );
}

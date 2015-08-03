// shaders/VS_ShadowMap.glsl
// shaders/VS_ShadowMap.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------

#version 150

#define SHADOWMAP_TRANSPARENT_MATERIALS

uniform mat4 im_ModelViewMatrix;
uniform mat4 im_ProjectionMatrix;
uniform mat4 im_ModelViewProjectionMatrix;
uniform mat3 im_NormalMatrix;

in vec3 iv_Vertex;

#ifdef SHADOWMAP_TRANSPARENT_MATERIALS
in vec2 iv_UvCoord;
out vec2 texCoords;
#endif

void main( void )
{
#ifdef SHADOWMAP_TRANSPARENT_MATERIALS
	texCoords = iv_UvCoord;
#endif
	gl_Position = im_ModelViewProjectionMatrix * vec4(iv_Vertex, 1.0);
}

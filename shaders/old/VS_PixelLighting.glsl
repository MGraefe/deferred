// shaders/old/VS_PixelLighting.glsl
// shaders/old/VS_PixelLighting.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------


#define TANGENT_CALC_IN_FRAGMENT

#ifndef TANGENT_CALC_IN_FRAGMENT
varying vec3 eyeVec;
varying vec3 lightVec;
#else
varying vec3 vertexPosition;
varying vec3 lightDir;
varying vec3 n;
varying vec3 t;
varying vec3 b;
#endif

// Used for shadow lookup
varying vec4 ShadowCoordHD;
varying vec4 ShadowCoordSD;

//uniform mat3 worldmatrix;
//uniform float normaldir;

attribute vec4 vTang;


void main( void )
{
	gl_Position = ftransform();
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
#ifndef TANGENT_CALC_IN_FRAGMENT
	vec3 n,t,b,vertexPosition,lightDir;
#endif
	n = normalize(gl_NormalMatrix * gl_Normal);
	t = normalize(gl_NormalMatrix * vTang.xyz);
	b = cross(n, t) * vTang.w;

	vertexPosition = vec3(gl_ModelViewMatrix * gl_Vertex);
	lightDir = normalize(gl_LightSource[0].position.xyz); // - vVertex;

#ifndef TANGENT_CALC_IN_FRAGMENT
	vec3 v;
	v.x = dot(lightDir, t);
	v.y = dot(lightDir, b);
	v.z = dot(lightDir, n);
	lightVec = normalize(v);

	v.x = dot(vertexPosition, t);
	v.y = dot(vertexPosition, b);
	v.z = dot(vertexPosition, n);
	eyeVec = normalize(v);
#endif

	ShadowCoordSD = gl_TextureMatrix[6] * gl_Vertex;
	ShadowCoordHD = gl_TextureMatrix[7] * gl_Vertex;

	//Fix ATI/NVIDIA Clip Plane Issues
	#ifdef __GLSL_CG_DATA_TYPES
		gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;
	#endif
	
	vec4 IFogColor = vec4( 0.3, 0.3, 0.3, 0.3 );
	gl_FrontColor  = gl_Color;
}

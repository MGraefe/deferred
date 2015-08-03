// shaders/VS_GBufferOut.glsl
// shaders/VS_GBufferOut.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------


#version 150

//uniform mat4 im_ModelViewMatrix;
//uniform mat4 im_ProjectionMatrix;
uniform mat4 im_ModelViewProjectionMatrix;
uniform mat3 im_NormalMatrix;

in vec3 iv_Vertex;
in vec3 iv_Normal;
in vec2 iv_UvCoord;
in vec4 iv_Tangent;
in float if_SkyVisibility;

out vec3 vNormal;
//out vec3 vViewPos;
out vec2 vTexCoord;
out mat3 mTangentSpace;
out float fSkyVisibility;

void main( void )
{
	vTexCoord = iv_UvCoord;
	gl_Position = im_ModelViewProjectionMatrix * vec4(iv_Vertex, 1.0);
	fSkyVisibility = if_SkyVisibility;
	
	vNormal = im_NormalMatrix * iv_Normal;
	//vViewPos = (im_ModelViewMatrix * vec4(iv_Vertex, 1.0)).xyz;
	
	vec3 n = vNormal;
	vec3 t = im_NormalMatrix * iv_Tangent.xyz;
	vec3 b = cross(n, t) * iv_Tangent.w;
	mat3 ntb = mat3x3(t,b,n);
	
	mTangentSpace = ntb;
	
	//Fix ATI/NVIDIA Clip Plane Issues
//#ifdef __GLSL_CG_DATA_TYPES
//	gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;
//#endif
}

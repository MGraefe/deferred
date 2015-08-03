// shaders/old/VS_DeferredLighting.glsl
// shaders/old/VS_DeferredLighting.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------


#version 150

uniform mat4 im_ModelViewMatrix;
uniform mat4 im_ProjectionMatrix;
//uniform mat3 im_NormalMatrix;
//uniform mat4 im_ShadowMatrix[3];
//uniform mat4 im_ViewSpaceMat;

in vec3 iv_Vertex;

//out vec2 texCoord;

//out flat mat4 mShadowSpaceMatrix0;
//out flat mat4 mShadowSpaceMatrix1;
//out flat mat4 mShadowSpaceMatrix2;

void main( void )
{
	gl_Position = im_ProjectionMatrix * im_ModelViewMatrix * vec4(iv_Vertex, 1.0);
	
	//mat4 mViewSpaceInv = inverse(im_ViewSpaceMat);
	//mShadowSpaceMatrix0 = im_ShadowMatrix[0] * mViewSpaceInv;
	//mShadowSpaceMatrix1 = im_ShadowMatrix[1] * mViewSpaceInv;
	//mShadowSpaceMatrix2 = im_ShadowMatrix[2] * mViewSpaceInv;

//Fix ATI/NVIDIA Clip Plane Issues
//NOTENOTE: Not needed in Post-Effect-Shaders
//#ifdef __GLSL_CG_DATA_TYPES
//	gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;
//#endif
}

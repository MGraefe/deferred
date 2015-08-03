// shaders/old/VS_PostEffectCompositor.glsl
// shaders/old/VS_PostEffectCompositor.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------

 
void main( void )
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position    = ftransform();

//Fix ATI/NVIDIA Clip Plane Issues
//#ifdef __GLSL_CG_DATA_TYPES
//	gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;
//#endif
}

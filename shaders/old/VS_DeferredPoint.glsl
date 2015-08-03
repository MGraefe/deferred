// shaders/old/VS_DeferredPoint.glsl
// shaders/old/VS_DeferredPoint.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------

#version 150

uniform mat4 im_ModelViewProjectionMatrix;

in vec3 iv_Vertex;

void main( void )
{
	gl_Position = im_ModelViewProjectionMatrix * vec4(iv_Vertex, 1.0);
}

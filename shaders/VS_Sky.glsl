// shaders/VS_Sky.glsl
// shaders/VS_Sky.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------


#version 150

uniform mat4 im_ModelViewProjectionMatrix;

in vec3 iv_Vertex;

out vec4 position;
 
void main( void )
{
	position = vec4(iv_Vertex, 1.0);
	gl_Position = im_ModelViewProjectionMatrix * vec4(iv_Vertex, 1.0);
}

// shaders/VS_Unlit.glsl
// shaders/VS_Unlit.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------


#version 150

uniform mat4 im_ModelViewProjectionMatrix;

in vec3 iv_Vertex;
in vec2 iv_UvCoord;

out vec2 vTexCoord;
 
void main( void )
{
	vTexCoord = iv_UvCoord;
	gl_Position = im_ModelViewProjectionMatrix * vec4(iv_Vertex, 1.0);
}

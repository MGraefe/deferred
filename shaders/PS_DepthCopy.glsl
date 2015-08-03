// shaders/PS_DepthCopy.glsl
// shaders/PS_DepthCopy.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------


#version 150

uniform sampler2D tex0;

in vec2 vTexCoord;

void main( void )
{
	gl_FragDepth = texture(tex0, vTexCoord).r;
}

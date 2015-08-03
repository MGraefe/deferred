// shaders/PS_ShadowMapVisualize.glsl
// shaders/PS_ShadowMapVisualize.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------


#version 150

uniform sampler2DArray tex0;
uniform float slice;

in vec2 vTexCoord;

out vec3 oc_Color1;

void main( void )
{
	oc_Color1 = vec3(texture(tex0, vec3(vTexCoord, slice)).r);
}

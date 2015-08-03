// shaders/PS_Sky.glsl
// shaders/PS_Sky.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------


#version 150

uniform samplerCube tex0;
uniform float brightness;

in vec4 position;
out vec3 oc_Color1;

void main( void )
{
	oc_Color1 = texture(tex0, position.xyz).rgb * brightness;
}

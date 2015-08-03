// shaders/PS_Unlit.glsl
// shaders/PS_Unlit.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------


#version 150

uniform sampler2D tex0;
uniform vec4 cDrawColor;
uniform bool bTexEnabled;
uniform bool bSingleChannelAlpha;

in vec2 vTexCoord;

out vec4 oc_Color1;


void main( void )
{
	vec4 outColor;
	if( bTexEnabled )
	{
		if( bSingleChannelAlpha )
			outColor = vec4(1.0, 1.0, 1.0, texture(tex0, vTexCoord).r) * cDrawColor;
		else
			outColor = texture(tex0, vTexCoord) * cDrawColor;
	}
	else
	{
		outColor = cDrawColor;
	}
	
	oc_Color1 = outColor;
}

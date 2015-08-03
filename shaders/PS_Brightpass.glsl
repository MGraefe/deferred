// shaders/PS_Brightpass.glsl
// shaders/PS_Brightpass.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------


#version 150

#include "includes/hdrFuncs.glsl"

uniform sampler2D tex0; //scene tex
uniform sampler2D tex1; //luminance
uniform int sourceLod;

in vec2 vTexCoord;

out vec4 oc_Color1;

const vec3 thresh = vec3(0.7, 0.7, 0.7);
const float desiredLum = 0.5;

void main(void)
{
	vec3 texColor = texelFetch(tex0, ivec2(gl_FragCoord.xy), sourceLod).rgb;

	float sceneLum = texelFetch(tex1, ivec2(0,0), 0).r;
	float exposure = desiredLum / sceneLum;
	texColor *= tonemap(exposure, brightMax);
	
	float lum = luminance(texColor);
	//vec3 color = max(texColor * lum - thresh, 0.0);
	vec3 color = max(texColor - thresh, 0.0);
	
	oc_Color1 = vec4(color, 1.0);
	//oc_Color1 = vec4(1,0,1,1);
}
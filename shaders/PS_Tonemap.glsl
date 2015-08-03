// shaders/PS_Tonemap.glsl
// shaders/PS_Tonemap.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------


#version 150

uniform sampler2D tex0; //scene tex
uniform sampler2D tex1; //scene depth
uniform sampler2D tex2; //bloom tex
uniform sampler2D tex3; //luminance tex
uniform int iBloomLodLevels;
uniform int iBloomLodStart;
uniform float fNearZ;
uniform float fFarZ;

const float bloomFactor = 2.0;
const float vignetteScale = 0.70;
const float vignetteFactor = 0.7;
const vec3 fogColor = vec3(0.7, 0.7, 0.7);
const float fogDensity = 1.0/15000.0;
const float desiredLum = 0.5;
const float gamma = (1.0 / 2.2);
const float saturation = 0.8;
const vec3 colorWeights = vec3(1.0, 1.0, 1.1);
const float brightness = 1.2;

in vec2 vTexCoord;

out vec3 oc_Color1;

#include "includes/linearizeDepth.glsl"
#include "includes/hdrFuncs.glsl"


float calcFogFactor(float density, float dist)
{
	const float MLOG2 = -1.442695;
	return clamp(exp2( density * density * dist * dist * MLOG2 ), 0.0, 1.0);
}

void main(void)
{
	//sample scene color
	vec3 sceneColor = texture(tex0, vTexCoord).rgb;
	
	//sample bloom color
	vec3 bloomColor = vec3(0,0,0);
	for( int i = iBloomLodStart; i < iBloomLodStart+iBloomLodLevels; ++i )
		bloomColor += textureLod(tex2, vTexCoord, float(i)).rgb;
	bloomColor /= float(iBloomLodLevels);
	
	//calculate vignette
	vec2 vignetteVector = (vTexCoord - 0.5) * 2.0 * vignetteScale;
	float vignetteDot = dot(vignetteVector, vignetteVector);
	float vignette = 1.0 - vignetteDot * vignetteDot * vignetteScale;
	
	//sample luminance and calculate exposure
	float exposure = desiredLum / texelFetch(tex3, ivec2(0,0), 0).r;

	vec3 color = sceneColor;
	
	//Apply fog
	float depth = texture(tex1, vTexCoord).r;
	if( depth < 1.0 )
	{
		float fogFactor = calcFogFactor(fogDensity, linearize_depth(depth));
		color = mix(fogColor, color, fogFactor);
	}
	
	
	//Apply tonemapping
	color *= tonemap(exposure, brightMax);
	
	//Apply bloom
	color += bloomColor * bloomFactor;
	
	//Apply rgb weights and brightness
	color *= colorWeights * brightness;
	
	//Apply saturation
	vec3 gsColor = vec3(grayscale(color.rgb));
	color = mix(gsColor, color, saturation);
	
	//Apply gamma correction
	color = pow(color, vec3(gamma));
	
	//Apply vignette
	color *= vignette;
	
	oc_Color1 = color;
	//oc_Color1 = bloomColor;
	//oc_Color1 = textureLod(tex2, sc, iBloomLodStart);
}
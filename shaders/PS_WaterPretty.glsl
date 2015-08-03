// shaders/PS_WaterPretty.glsl
// shaders/PS_WaterPretty.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------


#version 150

//Graphic options
#define WATER_DETAILS_HIGH
#ifdef WATER_DETAILS_HIGH
	#define WATER_SPECULARS
	#define FRESNEL_EXPENSIVE
#endif

//Texture Samplers
uniform sampler2D tex0; //water heightmap
uniform sampler2D tex1; //scene color
uniform sampler2D tex2; //scene depth
uniform samplerCube tex3; //reflection cube map

//Other useful Uniforms
uniform vec2 vScreenSize;
uniform float fTanHalfFov;
uniform float fNearZ;
uniform float fFarZ;
uniform vec3 sunDir;
uniform vec3 seaBright;
uniform vec3 seaDark;

//in vec3 vNormalSS;

const float pi = 3.141592653589;

//Output of vertex shader
in vec3 viewDir;
in vec2 uvCoords;
in vec3 normal;

//Output of this shader
out vec4 oc_Color;

#include "includes/DeferredFuncs.glsl"

// const vec3 seaDark = 	pow(vec3(17, 11, 11) / 255.0, vec3(2.2));
// const vec3 seaBright = 	pow(vec3(17, 11, 11) / 255.0, vec3(2.2));
const float reflectionStrength = 1.0;
const float specularExp = 200.0;

float fresnel(float facing)
{
	float r0 = 0.005;
	return max(r0 + (1.0 - r0) * pow(facing, 5), 0.0);
}

void main( void )
{
	//general things
	float NdotV = max(dot(normal, -viewDir), 0);
	float facing = 1.0 - NdotV;
	
	vec3 reflected = reflect(viewDir, normal);
	vec3 reflColor = texture(tex3, reflected).rgb * 1.3;
	
	float fresnelTerm = fresnel(facing);
	vec3 refrColor = mix(seaDark, seaBright, fresnelTerm);
	//vec3 refrColor = seaDark;
	
	float specular = pow(max(dot(reflected, sunDir), 0.0), specularExp) * 1.5;
	vec3 specularColor = vec3(1.0, 1.0, 1.0) * specular;
	
	vec3 finalColor = mix(refrColor, reflColor, fresnelTerm) + specularColor;
	
	oc_Color = vec4(finalColor, 1.0);
}

// shaders/PS_DeferredPoint.glsl
// shaders/PS_DeferredPoint.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------


#version 150

uniform vec3 vPointLightPos;
uniform vec4 vPointLightColor;
uniform vec4 vPointLightAttent;

uniform sampler2D tex0; //diffuse
uniform sampler2D tex1; //normal / depth
uniform sampler2D tex2; //shadow map
uniform sampler2DArrayShadow tex3; //depth

uniform vec2 vScreenSize;
uniform float fTanHalfFov;
uniform float fNearZ;
uniform float fFarZ;

out vec4 oc_Color;

const float pi = 3.141592653589;

#include "includes/DeferredFuncs.glsl"

float attentuate( vec4 lightData, float dist )
{
	float att =  1.0 / ( lightData.x + lightData.y*dist + lightData.z*dist*dist );
	float damping = 1.0 - (dist/lightData.w);
	return max(att * damping, 0.0);
}

void main( void )
{
	vec2 texCoord = gl_FragCoord.xy / vScreenSize;
	float fDepth = linearize_depth( texture( tex2, texCoord ).r );
	if( fDepth+1.0 >= fFarZ )
	{
		discard;
		return;
	}
	vec4 cTex0 = texture( tex0, texCoord );
	vec4 cTex1 = texture( tex1, texCoord );
			
	vec3 cTexColor = cTex0.rgb;
	float fSpec = cTex0.a;
	vec3 vNormal = decodeNormal(cTex1.rg);
	vec3 vFragPos = GetFragmentPos( fDepth );
	
	//Calulate light terms
	vec3 fragToLight = vPointLightPos - vFragPos;
	float dist = length(fragToLight);
	float attent = attentuate( vPointLightAttent, dist );
	vec3 lightDir = normalize(fragToLight);
	vec3 cFinal = cTexColor *
					vPointLightColor.rgb * 
					max(dot(vNormal, lightDir), 0.0) * 
					attent * 
					vPointLightColor.w;
	//cFinal.g += 0.2;
	oc_Color = vec4( cFinal, 1.0 );
}

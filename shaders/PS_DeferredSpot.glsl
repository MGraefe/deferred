// shaders/PS_DeferredSpot.glsl
// shaders/PS_DeferredSpot.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------


#version 150

uniform vec3 vLightPos;
uniform vec3 vLightDir;
uniform vec4 vLightColor;
uniform vec4 vLightAttent; //w is the damping factor
uniform vec2 vLightAngle; //x = angle in cos form, y = fading across angle, 1.0 produces a hard circle, 0.0 fades nicely

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

float lerp( float val, float A, float B, float C, float D)
{
	return C + (D - C) * (val - A) / (B - A);
}

float attentuate( vec4 lightData, float dist, float dotProd )
{
	float distAtt =  1.0 / ( lightData.x + lightData.y*dist + lightData.z*dist*dist );
	float distDamping = 1.0 - (dist/lightData.w);
	float angleAtt = max( vLightAngle.y, lerp(dotProd, 1.0, vLightAngle.x, 1.0, 0.0 ) ) * float(dotProd > vLightAngle.x);
	return max(distAtt * distDamping * angleAtt, 0.0);
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
	vec3 lightToFrag = vFragPos - vLightPos;
	float dist = length(lightToFrag);
	vec3 fragDir = lightToFrag/dist;
	float dotProd = dot( fragDir, vLightDir );
	float attent = attentuate( vLightAttent, dist, dotProd );
	
	vec3 cFinal = cTexColor *
					vLightColor.rgb * 
					max(dot(vNormal, -fragDir), 0.0) * 
					attent * 
					vLightColor.w;
	//cFinal.g += 0.1;
	oc_Color = vec4( cFinal, 1.0 );
}

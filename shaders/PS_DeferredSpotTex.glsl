// shaders/PS_DeferredSpotTex.glsl
// shaders/PS_DeferredSpotTex.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------


#version 150

uniform vec4 vLightColor;
uniform mat4 mLightSpace;
uniform vec3 vLightPos;
uniform vec4 vLightAttent;

uniform sampler2D tex0; //diffuse
uniform sampler2D tex1; //normal / depth
uniform sampler2D tex2; //shadow map
uniform sampler2DArrayShadow tex3; //depth
uniform sampler2D tex4; //light texture

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
	
	vec3 fragToLight = vLightPos - vFragPos;
	float dist = length(fragToLight);
	float intensity = max(dot(normalize(fragToLight), vNormal), 0.0);
	float attent = 1.0 / (vLightAttent.x + vLightAttent.y*dist + vLightAttent.z*dist*dist);
	vec4 vFragCoordsLight = mLightSpace * vec4(vFragPos, 1.0);
	vec3 normCoords = vFragCoordsLight.xyz / vFragCoordsLight.w;
	float visibility = float(normCoords.x > -1.0 && normCoords.x < 1.0 && normCoords.y > -1.0 && normCoords.y < 1.0 && normCoords.z > -1.0 && normCoords.z < 1.0);
	//float visibility = 1.0;
	vec3 light = cTexColor * texture(tex4, normCoords.xy * 0.5 + vec2(0.5)).xyz * vLightColor.xyz * vLightColor.w * visibility * intensity * attent;
	
	oc_Color = vec4( light, 1.0 );
}

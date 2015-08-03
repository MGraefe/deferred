// shaders/PS_DeferredLighting.glsl
// shaders/PS_DeferredLighting.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------


#version 150

#define SHADOW_CASCADES 4
#define SHADOW_PCF4
#define SPECULAR {(render.speculars)}
//#define COLORIZE_SPLITS



uniform vec3 vSunLightDir;
uniform vec4 vSunLightColor;
uniform vec4 vSunLightSpecularColor;
uniform vec3 cAmbientMin;
uniform vec3 cAmbientMax;
const float fSunSpecularFactor = 0.6;

uniform sampler2D tex0; //diffuse
uniform sampler2D tex1; //normal / depth
uniform sampler2D tex2; //Depth
uniform sampler2DArrayShadow tex3; //Shadow maps

uniform float shadowMapSizeR;

uniform vec2 vScreenSize;
uniform float fTanHalfFov;
uniform float fNearZ;
uniform float fFarZ;

//in vec2 texCoord;
uniform mat4 mShadowSpaceMatrix[SHADOW_CASCADES];

out vec4 oc_Color;

const float pi = 3.141592653589;
const float shadowDists[SHADOW_CASCADES] = float[SHADOW_CASCADES](54.9, 126.0, 293.8, 1000);



#include "includes/DeferredFuncs.glsl"

float offset_lookup( vec4 coords, vec2 offset )
{
	vec4 offsetCoords = vec4(coords.xy + offset * shadowMapSizeR, coords.z, coords.w);
	return texture( tex3, offsetCoords );
	//return dot( textureGather(tex3, coords.xyz, coords.w ), vec4(0.25) );
}

float sampleShadowPCF1( vec4 coords )
{
	return texture(tex3, coords);
}

float sampleShadowPCF4( vec4 coords )
{
	float d = 0.5;
	return (offset_lookup(coords, vec2(-d,-d))+
			offset_lookup(coords, vec2(-d, d))+
			offset_lookup(coords, vec2( d,-d))+
			offset_lookup(coords, vec2( d, d))) * 0.25;
}

float sampleShadowPCF16( vec4 coords )
{
	float sum = 0.0;
	float x = -1.5;
	float y = -1.5;
	for( int i = 0; i < 4; i++ )
	{
		for( int k = 0; k < 4; k++ )
		{
			sum += offset_lookup(coords, vec2(x, y));
			x += 1.0;
			y += 1.0;
		}
	}
	return sum / 16.0f;
}


#ifdef COLORIZE_SPLITS
vec4 splitcolor = vec4(0.0);
#endif

vec4 calcShadowmapCoords(vec4 fragpos)
{
	//calculate possible candiates
	vec4 tc[SHADOW_CASCADES];
	for( int i = 0; i < SHADOW_CASCADES; i++ )
	{
		tc[i] = mShadowSpaceMatrix[i] * fragpos;
		tc[i].w = tc[i].z /*- 0.0001*/;
		tc[i].z = float(i);
	}

	//check if candidate is valid
	const float low = (1.0/1024.0);
	const float high = 1.0-low;
	bool b[SHADOW_CASCADES];
	for( int i = 0; i < SHADOW_CASCADES; i++ )
		b[i] = tc[i].x > low && tc[i].y > low && tc[i].x < high && tc[i].y < high;

	//calculate least possible index
	vec4 t = vec4(0.0);
	for( int i = SHADOW_CASCADES-1; i >= 0; i-- )
		t = b[i] ? tc[i] : t;

#ifdef COLORIZE_SPLITS
	splitcolor[int(t.z+0.5)] = 1.0;
#endif

	return t;
}

#define SHADOW_BOUNDINGBOX

float shadowLookup( vec3 fragpos, float depth )
{
#ifdef SHADOW_BOUNDINGBOX
	vec4 t = calcShadowmapCoords(vec4(fragpos, 1.0));
	return sampleShadowPCF4(t);
#else
	float fIndex = 0;
	for( int i = 0; i < SHADOW_CASCADES; ++i )
		fIndex += float(depth > shadowDists[i]);
		
	if( fIndex > (float(SHADOW_CASCADES)-0.5) )
		return 1.0;
	else
	{	
		vec4 coords = mShadowSpaceMatrix[int(fIndex)] * vec4(fragpos,1.0);
		coords.w = coords.z;
		coords.z = fIndex;
		
		//return shadow2DArray(tex3, coords).x;
		return sampleShadowPCF4( coords );
	}
#endif
}


void main( void )
{
	vec2 texCoord = gl_FragCoord.xy / vScreenSize;
	vec4 cTex0 = texture( tex0, texCoord );
	float fDepth = linearize_depth( texture( tex2, texCoord ).r );
	if( fDepth+1.0 >= fFarZ )
	{
		oc_Color = cTex0;
		return;
	}

	vec4 cTex1 = texture( tex1, texCoord );
			
	vec3 cTexColor = cTex0.rgb;
	float fSpecular = cTex0.a;
	float fSmoothness = cTex1.b;
	float fSkyVisibility = cTex1.a;
	vec3 vNormal = decodeNormal(cTex1.rg);
	vec3 vViewPos = GetFragmentPos( fDepth );
	
	vec3 cAmbient = mix(cAmbientMin, cAmbientMax, fSkyVisibility);
	
	float lightFactor = dot(vNormal, vSunLightDir);
	float fShadowMapVal = 0.0;
	if( lightFactor > 0.0 )
		fShadowMapVal = shadowLookup(vViewPos, -vViewPos.z);
	
	if( fShadowMapVal > 0.01 )
	{
		vec3 cDiffuse = vSunLightColor.rgb * vSunLightColor.a * lightFactor * fShadowMapVal;
		vec3 eyeVec = normalize(vViewPos);
		vec3 cFinal = (cAmbient + cDiffuse) * cTexColor;
		
		#if SPECULAR == 1
			#ifdef PHONG
				vec3 vReflected = normalize(reflect(vSunLightDir, vNormal));
				vec3 cSpecular = vec3(1.0,1.0,1.0) * pow(max(dot(vReflected, eyeVec), 0.0), 100.0) * fSunSpecularFactor * fShadowMapVal;
				cFinal += cSpecular;
			#else
				//Blinn-Phong:
				vec3 halfVec = normalize(-eyeVec + vSunLightDir);
				float specularExp = fSmoothness * 255.0;
				vec3 cSpecular = vSunLightSpecularColor.rgb * fSpecular * pow(max(dot(vNormal, halfVec), 0.0), specularExp) * vSunLightSpecularColor.a * fShadowMapVal;
				cFinal += cSpecular;
			#endif
		#endif
		oc_Color = vec4( cFinal.rgb, 1.0 );
	}
	else
	{
		oc_Color = vec4( cAmbient*cTexColor, 1.0 );
	}

#ifdef COLORIZE_SPLITS
	oc_Color *= splitcolor;
#endif

	//oc_Color = vec4(vNormal, 1.0);
}

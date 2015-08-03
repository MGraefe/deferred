// shaders/PS_Water.glsl
// shaders/PS_Water.glsl
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
uniform sampler2D tex0; //water normal map1
uniform sampler2D tex1; //water normal map2
uniform sampler2D tex2; //scene color
uniform sampler2D tex3; //scene depth
uniform samplerCube tex4; //reflection cube map

//Other useful Uniforms
uniform vec2 vScreenSize;
uniform float fTanHalfFov;
uniform float fNearZ;
uniform float fFarZ;

//in vec3 vNormalSS;

const float pi = 3.141592653589;

//Output of vertex shader
in vec3 vWaterPosWS;
in vec2 vUvCoords1;
in vec2 vUvCoords2;
//in mat3 mTangentSpace;


//Output of this shader
out vec4 oc_Color;

//Contants
//const vec3 reflectionColor = vec3(243.0, 255.0, 242.0) * (1.0/255.0);
const vec3 reflectionColor = vec3(255.0, 255.0, 255.0) * (1.0/255.0);
const float waterNormalStrength = 1.5;
//const vec3 refractionColor = vec3(1.0, 49.0, 63.0) * (1.0/255.0);
const vec3 specularColor = vec3(1.0);
const float specularExp = 100.0;
const float specularStrength = 5.5 / waterNormalStrength;
const vec3 lightDir = normalize(vec3(0.0,-1.0,-1.0));
const float maxViewDistance = 1700.0;
const float fogEndDist = 150.0;
const vec3 fogStartColor = pow( vec3(1.0, 73.0, 39.0) * (1.0/255.0), vec3(2.2) );
const vec3 fogEndColor = pow( vec3(1.0, 40.0, 55.0) * (1.0/255.0), vec3(2.2) );

#include "includes/DeferredFuncs.glsl"


vec3 calcWaterNormal( float viewDistance )
{
	vec4 color1 = texture(tex0,vUvCoords1);
	vec4 color2 = texture(tex1,vUvCoords2);
	vec3 normal1 = color1.rgb * 2.0 - vec3(1.0);
	vec3 normal2 = color2.rgb * 2.0 - vec3(1.0);
	vec3 normal = normal1+normal2;
	normal.xy *= waterNormalStrength * (1.0-min(viewDistance/maxViewDistance,1.0));
	//vec3 normalSS = mTangentSpace * normal;
	return normalize(vec3(normal.x, normal.z, normal.y));
}

#ifdef WATER_SPECULARS
vec3 getSpecularHighlights( vec3 light, vec3 reflected, vec3 color )
{
	float f = 2.0;
	return pow( max(dot(-light,reflected),0.0), specularExp ) * color;
}
#endif

vec3 getWaterFog( vec3 baseColor, float dist )
{
	float fogFactor = clamp( dist / fogEndDist + 0.1, 0.0, 1.0 );
	//vec3 fogColor = mix( fogStartColor, fogEndColor, fogFactor );
	return mix( baseColor, fogEndColor, fogFactor );
	//return fogEndColor;
	//return vec3(baseColor);
}

void main( void )
{
	vec2 texCoord = gl_FragCoord.xy / vScreenSize;
	float depthRaw = texture( tex3, texCoord ).r;
	float depth = linearize_depth( depthRaw );
	//vec3 vFragPos = GetFragmentPos( fDepth );
	vec3 vEyeDirWS = normalize(vWaterPosWS);
	//float waterDist = length(vWaterPosWS);
	float waterDist = gl_FragCoord.z / gl_FragCoord.w;
	float waterGroundDist = depth - waterDist;
	
	//get normal from normal texture
	vec3 normal = calcWaterNormal( waterDist );
	vec3 reflected = reflect( vEyeDirWS, normal );
	reflected.y = max(-reflected.y, 0.0);
	vec3 reflColor = texture(tex4, reflected).rgb * reflectionColor;
	
#ifdef FRESNEL_EXPENSIVE
	float reflectIndex = clamp( 1.0 / pow(dot(normal,-vEyeDirWS)+1.0,8.0), 0.0, 1.0 );
#else
	float reflectIndex = 1.0 - clamp( dot(vEyeDirWS,normal) * -2.0, 0.0, 1.0 );
#endif
	
	vec2 distortion = vec2(normal.x, normal.z) * clamp(waterGroundDist*10.0, 0.0, 1.0) * 0.04;
	vec3 baseColor = textureLod(tex2, texCoord + distortion, 0.0).rgb;
	vec3 refractColor = getWaterFog(baseColor, waterGroundDist);
	vec3 finalColor = mix(refractColor, reflColor, reflectIndex);
		
#ifdef WATER_SPECULARS
	//vec3 specNormal = normal;
	//specNormal.xz *= 4.0;
	vec3 specRefl = normalize(reflect(vEyeDirWS, normalize(vec3(normal.x*specularStrength, normal.y, normal.z*specularStrength))));
	finalColor += getSpecularHighlights(lightDir,specRefl,specularColor);
#endif

	//float blending = clamp( (fDepth-waterDist)*0.1, 0.0, 1.0 );

	oc_Color = vec4(finalColor, 1.0);
	
	//oc_Color = vec4(normal, 1.0);
	//oc_Color = vec4(baseColor, 1.0);
	
	//oc_Color = vec4(reflected,1.0);
	//oc_Color = vec4(fDepth/fFarZ*0.1);
}

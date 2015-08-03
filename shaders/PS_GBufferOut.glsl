// shaders/PS_GBufferOut.glsl
// shaders/PS_GBufferOut.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------


#version 150
#include "includes/normalEncoding.glsl"

uniform sampler2D tex0;
uniform sampler2D tex1;

uniform bool bTranslucent;
uniform bool bNormalMap;
uniform float fDoubleSided; //-2.0 if face is double-sided, 0.0 if not
uniform float fSmoothness;
uniform float fSpecular;
uniform float fSkyVisFactor;

in vec3 vNormal;
//in vec3 vViewPos;
in vec2 vTexCoord;
in mat3 mTangentSpace;
in float fSkyVisibility;

out vec4 oc_Color1;
out vec4 oc_Color2;

vec3 GetNormal( void )
{
	if( bNormalMap )
	{
		vec3 normal = normalize( texture2D( tex1, vTexCoord ).xyz * 2.0 - vec3(1.0,1.0,1.0) );
		normal += normal * float(!gl_FrontFacing) * fDoubleSided;
		return mTangentSpace * normal;
	}
	else
	{
		vec3 normal = mTangentSpace * vec3(0.0,0.0,1.0);
		normal += normal * float(!gl_FrontFacing) * fDoubleSided;
		return normal;
	}
}


void main( void )
{
	vec4 col1 = texture( tex0, vTexCoord );
	
	if( bTranslucent && col1.a < 0.5 )
	{
		discard;
	}
	else
	{
		col1.a *= fSpecular;
		oc_Color1 = col1;
		oc_Color2 = vec4( encodeNormal(GetNormal()), fSmoothness, fSkyVisibility * fSkyVisFactor );
	}
}

// shaders/old/PS_PixelLighting.glsl
// shaders/old/PS_PixelLighting.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------

#define TANGENT_CALC_IN_FRAGMENT
#define SPECULAR_ENABLED

#ifndef TANGENT_CALC_IN_FRAGMENT
varying vec3 eyeVec;
varying vec3 lightVec;
#else
varying vec3 vertexPosition;
varying vec3 lightDir;
varying vec3 n;
varying vec3 t;
varying vec3 b;
#endif

varying vec4 ShadowCoordSD;
varying vec4 ShadowCoordHD;
 
uniform sampler2D Tex0; //Diffuse
uniform sampler2D Tex1; //Normal Map
uniform sampler2DShadow Tex2; //Shadow Map HD
uniform sampler2DShadow Tex3; //Shadow Map SD
uniform int iTranslucent;

vec3 AddFogColor( vec3 IColor )
{
	if( gl_Fog.density == 0.0 )
		return IColor;

	float fogFactor = 1.0;
	vec3 IFogColor;

	const float LOG2 = 1.442695;
	float z = gl_FragCoord.z / gl_FragCoord.w;
	fogFactor = exp2( -gl_Fog.density * gl_Fog.density * z * z * LOG2 );
	fogFactor = clamp(fogFactor, 0.0, 1.0);
	IFogColor = gl_Fog.color.rgb;

	return mix( IFogColor, IColor, fogFactor );
}


float ShadowLookup( sampler2DShadow tex, vec4 coords )
{
	float texsize = float(textureSize(tex, 0).x);
	coords.xy *= texsize;
	coords.xy -= fract(coords.xy);
	coords.xy *= 1.0/texsize;
	return (textureProjOffset( tex, coords, ivec2(-1,1)) +
			textureProjOffset( tex, coords, ivec2(1,1)) +
			textureProjOffset( tex, coords, ivec2(-1,-1)) +
			textureProjOffset( tex, coords, ivec2(1,-1)) ) * 0.25;
}


void main( void )
{

	vec4 ITexColor = texture2D(Tex0, gl_TexCoord[0].xy);
	vec4 INormColor = texture2D(Tex1, gl_TexCoord[0].xy);

	//Calculate final alpha and shine-factor
	float fAlpha = 1.0;
	float fShineFactor = ITexColor.a;
	if( iTranslucent == 1 )
	{
		if( ITexColor.a < 0.004 )
			discard;
		else
		{
			fAlpha = ITexColor.a;
			fShineFactor = INormColor.a;
		}
	}


#ifdef TANGENT_CALC_IN_FRAGMENT
	//Calculate Normal mapping things
	vec3 v, lightVec, eyeVec;
	v.x = dot(lightDir, t);
	v.y = dot(lightDir, b);
	v.z = dot(lightDir, n);
	lightVec = normalize(v);

	v.x = dot(vertexPosition, t);
	v.y = dot(vertexPosition, b);
	v.z = dot(vertexPosition, n);
	eyeVec = normalize(v);
#endif

	//Calculate shadow factor
	float shadow = 1.0;
	if( ShadowCoordHD.x >= 0.0 && ShadowCoordHD.x <= 1.0 && ShadowCoordHD.y >= 0.0 && ShadowCoordHD.y <= 1.0 )
		shadow = ShadowLookup(Tex2, ShadowCoordHD);
	else if( ShadowCoordSD.x >= 0.0 && ShadowCoordSD.x <= 1.0 && ShadowCoordSD.y >= 0.0 && ShadowCoordSD.y <= 1.0 )
		shadow = ShadowLookup(Tex3, ShadowCoordSD);
	
	//calculate normal
	vec3 norm = INormColor.rgb * 2.0 - vec3(1.0,1.0,1.0);
	if( dot(norm,norm) > 2.9 )
		norm = vec3(0.0, 0.0, 1.0);
	else
		norm = normalize(norm);

	//Calculate lighting components
	vec3 Reflected = normalize(reflect(lightVec, norm));
	vec3 IAmbient  = gl_LightSource[0].ambient.rgb;
	vec3 IDiffuse  = gl_LightSource[0].diffuse.rgb * max(dot(norm, lightVec), 0.0) * shadow;

#ifdef SPECULAR_ENABLED
	vec3 ISpecular = pow(max(dot(Reflected, eyeVec), 0.0), gl_FrontMaterial.shininess) * 
						gl_FrontMaterial.specular.rgb * 
						fShineFactor * 
						gl_LightSource[0].specular.rgb *
						shadow;
#endif

	vec3 IFinalColor   = (IAmbient + IDiffuse) * ITexColor.rgb * gl_Color.rgb;

#ifdef SPECULAR_ENABLED
	IFinalColor += ISpecular;
#endif

	IFinalColor = AddFogColor( IFinalColor );

	gl_FragData[0] = vec4( IFinalColor.rgb, fAlpha );

	//gl_FragColor = vec4( t.r, t.g, t.b, 1.0f );
}

// shaders/old/PS_WaterNoDepth.glsl
// shaders/old/PS_WaterNoDepth.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------


uniform sampler2D Tex0; //Reflection Image
uniform sampler2D Tex1; //Water Normal Map
uniform sampler2D TDepth; //Depth Buffer of Reflection
uniform sampler2D TexHeight; //Water Height map

//varying vec3 eyeVec;
//varying vec3 lightVec;
varying vec3 vertexPosition;
varying vec3 lightDir;
varying vec3 n;
varying vec3 t;
varying vec3 b;

uniform float ScreenWidthRezi;
uniform float ScreenHeightRezi;

uniform vec4 IWaterColor;

float far=4000.0;
float near = 1.0;

vec4 AddFogColor( vec4 IColor )
{
	if( gl_Fog.density == 0.0 )
		return IColor;

	float fogFactor = 1.0;
	vec4 IFogColor;

	const float LOG2 = 1.442695;
	float z = gl_FragCoord.z / gl_FragCoord.w;
	fogFactor = exp2( -gl_Fog.density * gl_Fog.density * z * z * LOG2 );
	fogFactor = clamp(fogFactor, 0.0, 1.0);
	IFogColor = gl_Fog.color;

	return mix( IFogColor, IColor, fogFactor );
}

void main( void )
{
	//Calculate Tangent space things
	vec3 v, lightVec, eyeVec;
	v.x = dot(lightDir, t);
	v.y = dot(lightDir, b);
	v.z = dot(lightDir, n);
	lightVec = normalize(v);

	v.x = dot(vertexPosition.xyz, t);
	v.y = dot(vertexPosition.xyz, b);
	v.z = dot(vertexPosition.xyz, n);
	eyeVec = normalize(v);

	//Smooth out the magnitude of distortion at the "edges" of the water:
	vec2 refl_coords = gl_FragCoord.xy;
	refl_coords.x *= ScreenWidthRezi;
	refl_coords.y *= ScreenHeightRezi;

	float fReflDepth = (2.0 * near) / (far + near - texture2D(TDepth, refl_coords).r * (far - near));
	float fWaterDepth = (2.0 * near) / (far + near - gl_FragCoord.z * (far - near));
	float fDist = fReflDepth - fWaterDepth;
	float fWeight = clamp( fDist*10.0 - 0.0, 0.0, 1.0 );


	//Calculate new normal map value from height map via paralax mapping
	float fHeightMapVal = texture2D( TexHeight, gl_TexCoord[0].xy ).r;
	float fNormalMapOffset = 0.5 * (2.0 * fHeightMapVal - 1.0);
	vec2 NormalMapTexCoords = gl_TexCoord[0].xy;// + eyeVec.xy * fNormalMapOffset;


	vec3 norm = texture2D(Tex1, NormalMapTexCoords).rgb * 2.0 - vec3(1.0);
	norm.x *= 5.0;
	norm.y *= 5.0;
	norm = normalize(norm);
	vec3 Reflected = normalize(reflect(lightVec, norm));

	vec4 ISpecular = vec4( pow(max(dot(Reflected, eyeVec), 0.0), 350.0) );

	//Get normal map parameters
	vec2 normal_dirs = ( texture2D(Tex1, NormalMapTexCoords).rg * 2.0 - vec2(1.0, 1.0) ) * 0.5;
	refl_coords += normal_dirs * fWeight * 1.0;
	vec4 ITexColor = texture2D(Tex0, refl_coords);
	vec4 IFinalColor = ITexColor * (1.0-IWaterColor.a) + IWaterColor * IWaterColor.a;
	IFinalColor += ISpecular;

	IFinalColor.a = 1.0;
	gl_FragData[0] = IFinalColor;

	//vec4 ITexColor = texture2D(Tex0, refl_coords.xy);

	//ISpecular.a = 0.5;
	//gl_FragColor = ITexColor;// + ISpecular;
	//gl_FragColor = vec4( dot(Reflected, eyeVec), 0, 0, 1 );
	//gl_FragColor = ISpecular;
}

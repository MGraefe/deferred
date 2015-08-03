// shaders/old/PS_PostEffectCompositor.glsl
// shaders/old/PS_PostEffectCompositor.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------

uniform sampler2D Tex0; //Normal Scene Color
uniform sampler2D Tex1; //Scene Depth
uniform sampler2D Tex2; //Bloom-Color
uniform sampler2D Tex3; //Shadow Map

uniform float fNear;
uniform float fFar;

const float LOG2 = 1.442695;

/*vec4 AddFogColor( vec4 IColor, float depth )
{
	if( gl_Fog.density == 0.0 )
		return IColor;

	if( depth == 1.0 )
		return IColor;

	float z = (2.0 * fNear * (fFar-fNear)) / (fFar + fNear - depth * (fFar - fNear));
	float fogFactor = exp2( -gl_Fog.density * gl_Fog.density * z * z * LOG2 );
	return mix( gl_Fog.color, IColor, clamp(fogFactor, 0.0, 1.0) );
}*/

float lerpclamped( float val, float A, float B, float C, float D)
{
	return clamp( (C+(D-C)*(val-A)/(B-A)), C, D );
}

void main( void )
{
	vec4 ISceneColor = texture2D( Tex0, gl_TexCoord[0].xy );
	vec4 IBloomColor = texture2D( Tex2, gl_TexCoord[0].xy );

	vec3 IBrightness = textureLod( Tex0, vec2(0.5,0.5), 100.0 ).rgb;
	float fBrightness = length(IBrightness);
	float fBloomFactor = lerpclamped( fBrightness, 1.0, 0.3, 1.0, 2.0 );

	vec4 IFinalColor = ISceneColor * fBloomFactor;
	IFinalColor.rgb += IBloomColor.rgb * fBloomFactor * 1.5;

	gl_FragColor = IFinalColor;

	//gl_FragColor = IBloomColor;
	//gl_FragColor = vec4( depthval, depthval, depthval, 1.0 );
}

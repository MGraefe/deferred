// shaders/old/PS_Bloom.glsl
// shaders/old/PS_Bloom.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------

uniform sampler2D tex0;

uniform int iBlurMode;
uniform int iBrightPass;
uniform vec4 IThreshold;

void main( void )
{

	if( iBrightPass == 1 )
	{
		//Simple box-shaped brightpass filter
		//Maybe we should use a different filter
		//vec4 IThreshold = vec4( 0.4, 0.4, 0.4, 0.0 );
		vec4 ITexColor = texture2D( tex0, gl_TexCoord[0].xy );

		gl_FragColor = max( ITexColor - IThreshold, 0.0 );
	}
	else
	{
		//fWeights[0] = 0.0005106;
		//fWeights[1] = 0.0232429;
		//fWeights[2] = 0.2297337;
		//fWeights[3] = 0.4930257;
		
		vec4 colorsum = vec4(0.0,0.0,0.0,0.0);

		if( iBlurMode == 0 )
		{
			float fWeights[4];
			fWeights[0]=0.070159327; fWeights[1]=0.131074879; fWeights[2]=0.190712824; fWeights[3]=0.216105941;
			float PixelWidth = 1.0 / float(textureSize(tex0, 0).x);
			vec2 ofs = vec2(PixelWidth, 0.0);
			
			colorsum += texture2D( tex0, gl_TexCoord[0].xy - ofs * 3.0 ) * fWeights[0];
			colorsum += texture2D( tex0, gl_TexCoord[0].xy - ofs * 2.0 ) * fWeights[1];
			colorsum += texture2D( tex0, gl_TexCoord[0].xy - ofs * 1.0 ) * fWeights[2];
			colorsum += texture2D( tex0, gl_TexCoord[0].xy			   ) * fWeights[3];
			colorsum += texture2D( tex0, gl_TexCoord[0].xy + ofs * 1.0 ) * fWeights[2];
			colorsum += texture2D( tex0, gl_TexCoord[0].xy + ofs * 2.0 ) * fWeights[1];
			colorsum += texture2D( tex0, gl_TexCoord[0].xy + ofs * 3.0 ) * fWeights[0];
		}
		else
		{
			float fWeights[4];
			fWeights[0]=0.000058525; fWeights[1]=0.009618931; fWeights[2]=0.205399653; fWeights[3]=0.569845784;

			float PixelHeight = 1.0 / float(textureSize(tex0, 0).y);
			vec2 ofs = vec2(0.0, PixelHeight);
			
			//colorsum += texture2D( tex0, gl_TexCoord[0].xy - ofs * 3.0 ) * fWeights[0];
			//colorsum += texture2D( tex0, gl_TexCoord[0].xy - ofs * 2.0 ) * fWeights[1];
			colorsum += texture2D( tex0, gl_TexCoord[0].xy - ofs * 1.0 ) * fWeights[2];
			colorsum += texture2D( tex0, gl_TexCoord[0].xy			   ) * fWeights[3];
			colorsum += texture2D( tex0, gl_TexCoord[0].xy + ofs * 1.0 ) * fWeights[2];
			//colorsum += texture2D( tex0, gl_TexCoord[0].xy + ofs * 2.0 ) * fWeights[1];
			//colorsum += texture2D( tex0, gl_TexCoord[0].xy + ofs * 3.0 ) * fWeights[0];
		}
		
		gl_FragColor = colorsum;
	}
}

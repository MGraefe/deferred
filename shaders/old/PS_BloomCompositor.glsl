// shaders/old/PS_BloomCompositor.glsl
// shaders/old/PS_BloomCompositor.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------

uniform sampler2D Tex0;
uniform sampler2D Tex1;
uniform sampler2D Tex2;
uniform sampler2D Tex3;
uniform sampler2D Tex4;

void main( void )
{
	//vec4 Color =	texture2D( Tex0, gl_TexCoord[0].xy ) + 
	//				texture2D( Tex1, gl_TexCoord[0].xy ) +
	//				texture2D( Tex2, gl_TexCoord[0].xy ) + 
	//				texture2D( Tex3, gl_TexCoord[0].xy ) + 
	//				texture2D( Tex4, gl_TexCoord[0].xy );

	//Color *= 1.0/5.0;
	gl_FragColor = texture2D( Tex4, gl_TexCoord[0].xy ) * 0.25 + 
				   texture2D( Tex3, gl_TexCoord[0].xy ) * 0.25 + 
				   texture2D( Tex2, gl_TexCoord[0].xy ) * 0.25 + 
				   texture2D( Tex1, gl_TexCoord[0].xy ) * 0.25;

	//gl_FragColor = texture2D( Tex4, gl_TexCoord[0].xy );
}

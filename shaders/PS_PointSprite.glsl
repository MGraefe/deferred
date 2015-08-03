// shaders/PS_PointSprite.glsl
// shaders/PS_PointSprite.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------


#version 150

uniform sampler2D tex0;

out vec4 oc_Color1;

void main( void )
{
	oc_Color1 = texture( tex0, gl_PointCoord );
	//oc_Color1 = vec4(1.0, 1.0, 1.0, texture( tex0, gl_PointCoord ).r);
	
	//if( oc_Color1.a < 0.5 )
	//	discard;
}

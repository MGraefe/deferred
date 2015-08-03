// shaders/PS_ShadowMap.glsl
// shaders/PS_ShadowMap.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------


#version 150

#define SHADOWMAP_TRANSPARENT_MATERIALS

#ifdef SHADOWMAP_TRANSPARENT_MATERIALS
uniform sampler2D tex0;
uniform bool bTranslucent;

in vec2 texCoords;
#endif

void main( void )
{
#ifdef SHADOWMAP_TRANSPARENT_MATERIALS
	if( bTranslucent && texture(tex0, texCoords).a < 0.5 )
		discard;
#endif

	//depth gets written automatically.
}

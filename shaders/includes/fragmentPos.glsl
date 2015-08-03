// shaders/includes/fragmentPos.glsl
// shaders/includes/fragmentPos.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------


//#include "test.glsl"
/*#include "test.glsl"*/

vec3 GetFragmentPos( float fDepth )
{
	vec2 fragCoord = vec2( 	(gl_FragCoord.x - vScreenSize.x*0.5) / vScreenSize.y * 2.0,
							(gl_FragCoord.y - vScreenSize.y*0.5) / vScreenSize.y * 2.0);
	vec3 dir = vec3( fTanHalfFov*fragCoord.x, fTanHalfFov*fragCoord.y, -1.0 );
	return dir * fDepth;
}
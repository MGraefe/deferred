// shaders/includes/linearizeDepth.glsl
// shaders/includes/linearizeDepth.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------


//#include "test.glsl"
/*#include "test.glsl"*/

float linearize_depth( float depth )
{
	float depthNorm = (2.0 * fNearZ) / (fFarZ + fNearZ - (2.0*depth-1.0) * (fFarZ - fNearZ));
	return depthNorm * (fFarZ-fNearZ);// + fNearZ;
}
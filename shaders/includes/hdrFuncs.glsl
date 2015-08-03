// shaders/includes/hdrFuncs.glsl
// shaders/includes/hdrFuncs.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------


const float brightMax = 1.0;
const vec3 brightnessWeights = vec3(0.299, 0.587, 0.144);

float tonemap(float exposure, float brightMax)
{
	return exposure * (exposure/brightMax + 1.0) / (exposure + 1.0) * 0.9;
}

float grayscale(vec3 color)
{
	return dot(color, brightnessWeights);
}

float luminance(vec3 color)
{
	return grayscale(color);
}



// shaders/PS_Exposure.glsl
// shaders/PS_Exposure.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------


#version 150

uniform float frametime;
uniform sampler2D texLastLum;
uniform sampler2D texScene;

out float oc_Color1;
float maxSceneLum = 0.6;

void main(void)
{
	float lastLum = texelFetch(texLastLum, ivec2(0,0), 0).r;
	/*
	vec3 sceneColor = vec3(0, 0, 0);
	ivec2 size = textureSize(texScene, 4);
	for(int i = 0; i < size.x; i++)
		for(int j = 0; j < size.y; j++)
			sceneColor += texelFetch(texScene, ivec2(i,j), 4).rgb;
	sceneColor /= size.x * size.y;
	*/
	vec3 sceneColor = textureLod(texScene, vec2(0.5, 0.5), 8).rgb;
	
	float currentLum = dot(sceneColor, vec3(0.299, 0.587, 0.114));
	//gamma correct luminance
	currentLum = min(pow(currentLum, 1.0/1.5), maxSceneLum);
	

	float lum = lastLum + (currentLum - lastLum) * (1.0f - pow(0.98f, 30.0f * frametime));

	oc_Color1 = lum;
	//oc_Color1 = 1.0;
}

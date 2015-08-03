// shaders/VS_WaterPretty.glsl
// shaders/VS_WaterPretty.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------

#version 150


uniform mat4 im_ModelViewMatrix;
uniform mat4 im_ProjectionMatrix;
uniform mat4 im_ModelViewProjectionMatrix;
uniform mat3 im_NormalMatrix;
uniform vec4 meshPosScale;
uniform float texFactor;
uniform vec3 camPos;

uniform sampler2D tex0; //height map


in vec3 iv_Vertex;

out vec3 viewDir;
out vec2 uvCoords;
out vec3 normal;

void main( void )
{
	vec3 vertWorld = iv_Vertex * meshPosScale.w + meshPosScale.xyz;
	uvCoords = vertWorld.xz * texFactor;
	float h = texture(tex0, uvCoords).r;
	vertWorld.y += h;
	
	vec2 h1 = vec2(textureOffset(tex0, uvCoords, ivec2(-1,0)).r, textureOffset(tex0, uvCoords, ivec2(0,-1)).r);
	vec2 h2 = vec2(textureOffset(tex0, uvCoords, ivec2(1,0)).r, textureOffset(tex0, uvCoords, ivec2(0,1)).r);
	float d = 1.0 / (float(textureSize(tex0, 0).x) * texFactor);
	normal.xz = (h1 - h2) / (2.0 * d);
	normal.y = 1.0;
	normal = normalize(normal);
	viewDir = normalize(vertWorld - camPos);
	
	gl_Position = im_ModelViewProjectionMatrix * vec4(vertWorld, 1.0);
}

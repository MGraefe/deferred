// shaders/VS_Water.glsl
// shaders/VS_Water.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------

#version 150

uniform mat4 im_ModelViewMatrix;
uniform mat4 im_ProjectionMatrix;
uniform mat3 im_NormalMatrix;

in vec3 iv_Vertex;
in vec3 iv_Normal;
in vec2 iv_UvCoord;
in vec4 iv_Tangent;

out vec3 vWaterPosWS;
out vec2 vUvCoords1;
out vec2 vUvCoords2;

void main( void )
{
	vec4 posSS = im_ModelViewMatrix * vec4(iv_Vertex, 1.0);
	gl_Position = im_ProjectionMatrix * posSS;
	vWaterPosWS = transpose(im_NormalMatrix) * posSS.xyz;
	vUvCoords1 = iv_UvCoord;
	vUvCoords2 = iv_Normal.xy;
	
	//vec3 n = vec3(0.0,1.0,0.0);
	//vec3 t = im_NormalMatrix * iv_Tangent.xyz;
	//vec3 b = cross(n, t) * iv_Tangent.w;
	//mTangentSpace = mat3x3(t,b,n);
}

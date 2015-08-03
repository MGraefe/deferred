// shaders/old/VS_WaterNoDepth.glsl
// shaders/old/VS_WaterNoDepth.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------


//varying vec3 eyeVec;
//varying vec3 lightVec;

varying vec3 vertexPosition;
varying vec3 lightDir;
varying vec3 n;
varying vec3 t;
varying vec3 b;

void main( void )
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position    = ftransform();

	vertexPosition = vec3(gl_ModelViewMatrix * gl_Vertex);
	lightDir = normalize(vec3(gl_LightSource[0].position.xyz));

	n = normalize(gl_NormalMatrix * gl_Normal);
	t = normalize(gl_NormalMatrix * vec3(0,0,1));
	b = -cross(n, t);

}

// shaders/VS_Blur.glsl
// shaders/VS_Blur.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------


#version 150

uniform mat4 im_ModelViewMatrix;
uniform mat4 im_ProjectionMatrix;

uniform sampler2D tex0;
uniform vec2 vBlurDir;
uniform float lodLevel;

in vec3 iv_Vertex;
in vec2 iv_UvCoord;

out coords
{
	vec4 ofs[4];
};

//const float offset[3] = float[]( 0.0, 1.3846153846, 3.2307692308 );
const float offset[4] = float[4]( 0.0, 1.4, 3.2666666667, 5.1333333333);

 
void main( void )
{
	vec2 dir = vBlurDir / vec2(textureSize(tex0, int(lodLevel+0.5))); //reziproc texture size
	ofs[0].xy = iv_UvCoord;
	for( int i = 1; i < 4; ++i )
	{
		ofs[i].xy = iv_UvCoord + dir * offset[i];
		ofs[i].zw = iv_UvCoord - dir * offset[i];
	}
	
	gl_Position = im_ProjectionMatrix * im_ModelViewMatrix * vec4(iv_Vertex, 1.0);
}

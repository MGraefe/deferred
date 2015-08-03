// shaders/VS_AA.glsl
// shaders/VS_AA.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------



#version 150

uniform mat4 im_ModelViewMatrix;
uniform mat4 im_ProjectionMatrix;

uniform sampler2D tex0; //Scene color

in vec3 iv_Vertex;
in vec2 iv_UvCoord;

out coords
{
	vec4 t0; // center
	vec4 t1; // top left/right
	vec4 t2; // center left/right
	vec4 t3; // bottom left/right
	vec4 t4; // top/bottom center
};

void main(void)
{
	vec2 p = vec2(1.0) / vec2(textureSize(tex0, 0));
	vec4 uv = vec4(iv_UvCoord.x, iv_UvCoord.y, iv_UvCoord.x, iv_UvCoord.y);	
	
	t0 = uv; //center
	t1 = uv + vec4(-p.x, -p.y,  p.x, -p.y); //top left/right
	t2 = uv + vec4(-p.x,  0,    p.x,  0  ); //center left/right
	t3 = uv + vec4(-p.x,  p.y,  p.x,  p.y); //bottom left/right
	t4 = uv + vec4( 0,   -p.y,  0,    p.y); //top/bottom center
	
	gl_Position = im_ProjectionMatrix * im_ModelViewMatrix * vec4(iv_Vertex, 1.0);
}

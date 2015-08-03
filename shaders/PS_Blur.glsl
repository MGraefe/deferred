// shaders/PS_Blur.glsl
// shaders/PS_Blur.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------


#version 150

uniform sampler2D tex0;
uniform float lodLevel;

in coords
{
	vec4 ofs[4];
};

out vec4 oc_Color1;

//const float weight[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703 );
const float weight[4] = float[4](0.2094982298, 0.3055182517, 0.0833231596, 0.0064094738);



void main(void)
{
	oc_Color1 = 	textureLod(tex0, ofs[0].xy,	lodLevel)*weight[0] +
					textureLod(tex0, ofs[1].xy, lodLevel)*weight[1] +
					textureLod(tex0, ofs[1].zw, lodLevel)*weight[1] +
					textureLod(tex0, ofs[2].xy, lodLevel)*weight[2] +
					textureLod(tex0, ofs[2].zw, lodLevel)*weight[2] +
					textureLod(tex0, ofs[3].xy, lodLevel)*weight[3] +
					textureLod(tex0, ofs[3].zw, lodLevel)*weight[3];
}
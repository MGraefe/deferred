// shaders/PS_AA.glsl
// shaders/PS_AA.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------


#version 150

uniform sampler2D tex0; //Scene color
uniform sampler2D tex1; //Scene depth
uniform sampler2D tex2; //GBuffer Normal
uniform float fNearZ;
uniform float fFarZ;


const vec2 e_barrier = vec2(0.8, 0.00001); //x = normal barrier, y = depth barrier
const vec2 e_weights = vec2(0.25, 0.25); //x = normal weight, y = depth weight
const vec2 e_kernel = vec2(1.0, 1.0); //kernel size in pixels

in coords
{
	vec4 t0; // center
	vec4 t1; // top left/right
	vec4 t2; // center left/right
	vec4 t3; // bottom left/right
	vec4 t4; // top/bottom center
};

out vec4 oc_Color1;

float linearize_depth( float depth )
{
	float depthNorm = (2.0 * fNearZ) / (fFarZ + fNearZ - (2.0*depth-1.0) * (fFarZ - fNearZ));
	return depthNorm * (fFarZ-fNearZ);
}

vec3 getNormal(vec2 t)
{
	return normalize(texture(tex2, t).rgb * 2.0 - vec3(1.0));
}

float getDepth(vec2 t)
{
	return texture(tex1, t).r;
}

void main(void)
{
	//Normal discontinuity filter
	vec3 nc = getNormal(t0.xy);
	vec4 n1 = vec4(	dot(nc, getNormal(t1.xy)),
					dot(nc, getNormal(t4.xy)),
					dot(nc, getNormal(t1.zw)),
					dot(nc, getNormal(t2.zw)));
	vec4 n2 = vec4(	dot(nc, getNormal(t3.zw)),
					dot(nc, getNormal(t4.zw)),
					dot(nc, getNormal(t3.xy)),
					dot(nc, getNormal(t2.xy)));
	n1 = abs(n1 - n2);
	vec4 normalResults = step(0.4, n1);
	
	//Depth - compute gradiental difference
	vec4 dc = vec4(getDepth(t0.xy));
	vec4 d1 = vec4(getDepth(t1.xy), getDepth(t4.xy), getDepth(t1.zw), getDepth(t2.zw));
	vec4 d2 = vec4(getDepth(t3.zw), getDepth(t4.zw), getDepth(t3.xy), getDepth(t2.xy));
	d1 = abs(d1 - dc);
	d2 = abs(dc - d2);
	vec4 maxDeltas = max(d1, d2);
	vec4 minDeltas = max(min(d1, d2), 0.00001);
	vec4 depthResults = step(minDeltas * 25.0, maxDeltas);
	
	float w = dot(max(normalResults, depthResults), vec4(0.25))*1.0;
	
	//sample color
	vec2 offset = t0.xy * (1.0 - w);
	
	vec4 c =
			//texture(tex0, t0.xy) +
			texture(tex0, offset + t1.xy * w) +
			texture(tex0, offset + t1.zw * w) +
			//texture(tex0, offset + t2.xy * w) +
			//texture(tex0, offset + t2.zw * w) +
			texture(tex0, offset + t3.xy * w) +
			texture(tex0, offset + t3.zw * w);
			//texture(tex0, offset + t4.xy * w) +
			//texture(tex0, offset + t4.zw * w);
			
	oc_Color1 = c * (1.0/4.0);
	
	//oc_Color1 = vec4(w, w, w, 1.0);
}

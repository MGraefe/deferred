// shaders/PS_BloomComplete.glsl
// shaders/PS_BloomComplete.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------


#version 150
#define DISCRETE 0

uniform sampler2D tex0;
uniform vec2 vBlurDir;
uniform vec4 cBrightpassSubstract;
uniform float fLodLevel;

in vec2 vTexCoord;

out vec4 oc_Color1;

#if DISCRETE == 1
#define LOOPCOUNT 9
const float offset[9] = float[9] ( -4.0, -3.0, -2.0, -1.0, 0.0, 1.0, 2.0, 3.0, 4.0 );
const float weight[9] = float[9]( 0.0162162162, 0.0540540541, 0.1216216216, 0.1945945946, 0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162 );
#else
#define LOOPCOUNT 5
const float offset[5] = float[5]( -3.2307692308, -1.3846153846, 0.0, 1.3846153846, 3.2307692308 );
const float weight[5] = float[5]( 0.0702702703, 0.3162162162, 0.2270270270, 0.3162162162, 0.0702702703 );
#endif

void main(void)
{
	vec2 samplePos = gl_FragCoord.xy;
	vec2 tsr = 1.0 / vec2(textureSize(tex0,int(fLodLevel))); //tex size reziproc
	
	//vec4 brightpassCenter = clamp( textureLod(tex0, samplePos*tsr, fLodLevel) + cBrightpassSubstract, 0.0, 1.0);
	//if( dot(brightpassCenter.rgb, brightpassCenter.rgb) > 0.1 )
	//{
	//	oc_Color1 = vec4(0,0,0,1);
	//}
	//else
	//{	
		for( int i = 0; i < LOOPCOUNT; ++i )
			for( int k = 0; k < LOOPCOUNT; ++k )
				oc_Color1 += (textureLod(tex0, (samplePos + vec2(offset[i], offset[k]))*tsr, fLodLevel) + cBrightpassSubstract) * weight[i]*weight[k];
	//}		
	//oc_Color1 = vec4(1.0, 0.0, 1.0, 1.0);
}
// shaders/includes/normalEncoding.glsl
// shaders/includes/normalEncoding.glsl
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------



vec2 encodeNormal(vec3 n)
{
    float p = 1.0 / sqrt(n.z * 8.0 + 8.0);
    return n.xy * p + vec2(0.5);
}

vec3 decodeNormal(vec2 enc)
{
    vec2 fenc = enc * 4.0 - vec2(2.0);
    float f = dot(fenc,fenc);
    float g = sqrt(1.0 - f*0.25);
    vec3 n;
    n.xy = fenc*g;
    n.z = 1.0 - f*0.5;
    return n;
}
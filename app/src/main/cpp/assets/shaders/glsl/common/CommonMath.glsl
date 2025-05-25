
#ifndef BP_COMMON_MATH_H
#define BP_COMMON_MATH_H

// GLSL: emulate HLSL-style select
vec2 select(bvec2 cond, vec2 a, vec2 b) {
    return vec2(
        cond.x ? a.x : b.x,
        cond.y ? a.y : b.y
    );
}

// 可扩展版本（适用于 vec3）
vec3 select(bvec3 cond, vec3 a, vec3 b) {
    return vec3(
        cond.x ? a.x : b.x,
        cond.y ? a.y : b.y,
        cond.z ? a.z : b.z
    );
}

// 可扩展版本（适用于 vec4）
vec3 select(bvec4 cond, vec4 a, vec4 b) {
    return vec4(
        cond.x ? a.x : b.x,
        cond.y ? a.y : b.y,
        cond.z ? a.z : b.z,
        cond.w ? a.w : b.w
    );
}


// 可扩展版本（适用于 float 标量）
float select(bool cond, float a, float b) {
    return cond ? a : b;
}

float rsqrt(float x) {
    return 1.0 / sqrt(x);
}

float square(float x) {
    return x * x;
}

float saturate(float x) {
    return clamp(x, 0.0, 1.0);
}

#endif
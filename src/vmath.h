#pragma once
#include "immintrin.h"
#define INLINE __attribute__((always_inline)) inline
#define shuffle_mask(x, y, z, w) ((x) | ((y) << 2) | ((z) << 4) | ((w) << 6))

typedef float vec2 __attribute__ ((__vector_size__ (8), __may_alias__));
typedef __m128 vec4;
typedef __m256 vec8;

INLINE vec4 v_yzxw(vec4 v)
{
    return _mm_permute_ps(v, shuffle_mask(1, 2, 0, 3));
}

INLINE vec4 fmadd(vec4 a, vec4 b, vec4 c)
{
    return _mm_fmadd_ps(a, b, c);
}

INLINE vec4 fmsub(vec4 a, vec4 b, vec4 c)
{
    return _mm_fmsub_ps(a, b, c);
}

INLINE vec4 cross(vec4 a, vec4 b)
{
    return v_yzxw(fmsub(a, v_yzxw(b), b * v_yzxw(a)));
}

INLINE vec4 dot(vec4 a, vec4 b)
{
    return _mm_dp_ps(a, b, 255);
}

INLINE vec4 len2(vec4 a)
{
    return dot(a, a);
}

INLINE vec4 len(vec4 a)
{
    return _mm_sqrt_ps(len2(a));
}

INLINE vec4 norm(vec4 a)
{
    return a * _mm_rsqrt_ps(len2(a));
}


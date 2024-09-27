#ifndef MATH_H
#define MATH_H

#ifndef FP
#define FP float
#endif

// 2D vector
typedef union {
    struct {
        FP x, y;
    };

    struct {
        FP u, v;
    };

    FP e[2];
} vec2d;

// 2D vector functions
vec2d vec2d_new(FP x, FP y);
vec2d vec2d_add(vec2d a, vec2d b);
vec2d vec2d_sub(vec2d a, vec2d b);
vec2d vec2d_mul(vec2d a, FP b);
vec2d vec2d_div(vec2d a, FP b);
FP vec2d_dot(vec2d a, vec2d b);
FP vec2d_mag(vec2d a);
vec2d vec2d_norm(vec2d a);

// 3D vector
typedef union {
    struct {
        FP x, y, z;
    };

    struct {
        FP r, g, b;
    };
    
    struct {
        FP i, j, k;
    };

    FP e[3];
} vec3d;

// 3D vector functions
vec3d vec3d_new(FP x, FP y, FP z);
vec3d vec3d_add(vec3d a, vec3d b);
vec3d vec3d_sub(vec3d a, vec3d b);
vec3d vec3d_mul(vec3d a, FP b);
vec3d vec3d_div(vec3d a, FP b);
FP vec3d_dot(vec3d a, vec3d b);
vec3d vec3d_cross(vec3d a, vec3d b);
FP vec3d_mag(vec3d a);
vec3d vec3d_norm(vec3d a);


#endif
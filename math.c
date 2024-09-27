#include "math.h"

#include <math.h> // libc

vec2d vec2d_new(FP x, FP y) {
    vec2d result;
    result.x = x;
    result.y = y;
    return result;
}

vec2d vec2d_add(vec2d a, vec2d b) {
    vec2d result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    return result;
}

vec2d vec2d_sub(vec2d a, vec2d b) {
    vec2d result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    return result;
}

vec2d vec2d_mul(vec2d a, FP b) {
    vec2d result;
    result.x = a.x * b;
    result.y = a.y * b;
    return result;
}

vec2d vec2d_div(vec2d a, FP b) {
    vec2d result;
    result.x = a.x / b;
    result.y = a.y / b;
    return result;
}

FP vec2d_dot(vec2d a, vec2d b) {
    return a.x * b.x + a.y * b.y;
}

FP vec2d_mag(vec2d a) {
    return sqrt(a.x * a.y + a.y * a.y);
}

vec2d vec2d_norm(vec2d a) {
    FP magnitude = vec2d_mag(a);
    return vec2d_div(a, magnitude);
}

vec3d vec3d_new(FP x, FP y, FP z) {
    vec3d result;
    result.x = x;
    result.y = y;
    result.z = z;
    return result;
}

vec3d vec3d_add(vec3d a, vec3d b) {
    vec3d result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    return result;
}

vec3d vec3d_sub(vec3d a, vec3d b) {
    vec3d result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    return result;
}

vec3d vec3d_mul(vec3d a, FP b) {
    vec3d result;
    result.x = a.x * b;
    result.y = a.y * b;
    result.z = a.z * b;
    return result;
}

vec3d vec3d_div(vec3d a, FP b) {
    vec3d result;
    result.x = a.x / b;
    result.y = a.y / b;
    result.z = a.z / b;
    return result;
}

FP vec3d_dot(vec3d a, vec3d b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3d vec3d_cross(vec3d a, vec3d b) {
    vec3d result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    return result;
}

FP vec3d_mag(vec3d a) {
    return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

vec3d vec3d_norm(vec3d a) {
    FP magnitude = vec3d_mag(a);
    return vec3d_div(a, magnitude);
}
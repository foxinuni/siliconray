#ifndef RENDERING_H
#define RENDERING_H

#include "universe.h"
#include <stdbool.h>

#define PI 3.14159265359
#define FOV 90

#ifndef THREADS
#define THREADS 4
#endif

#if THREADS < 1
#error "THREADS must be greater than 0!"
#endif

typedef struct {
    unsigned int width, height;
    unsigned char* image;
} image_t;

typedef struct {
    vec3d camera;
    vec3d light;
    vec3d target;
    float distance;
} raycast_t;

typedef void (*frame_cb)(image_t* image, int frame);

typedef struct {
    atom_t** universe;
    frame_cb callback;
    unsigned int width, height;
    int atoms;
    int frames;
    int from, to;
} frame_gen_t;

bool generate_frame(image_t* image, raycast_t* raycast, atom_t** universe, int atoms);
bool generate_frames(atom_t** universe, int atoms, unsigned int width, unsigned int height, frame_cb callback, int frames);

#endif
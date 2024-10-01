#ifndef UNIVERSE_H
#define UNIVERSE_H

#ifdef DEBUG
#define DPRINT_ATOM(atom) print_atom(atom)
#else
#define DPRINT_ATOM(atom)
#endif

#include <stdbool.h>
#include "math.h"

typedef enum {
    SPHERE,
    PLANE,
    CYLINDER,
} atom_kind;

typedef struct {
    atom_kind kind;
    void* properties;
} atom_t;

typedef struct {
    vec3d position;
    float radius;
} sphere_t;

typedef struct {
    vec3d position;
    vec3d normal;
} plane_t;

typedef struct {
    vec3d start;
    vec3d end;
    float radius;
} cylinder_t;

// destroys the atom independent of kind
bool raycast_atom(atom_t* atom, vec3d origin, vec3d direction, float* distance);
vec3d raycast_universe(atom_t* atoms[], int count, vec3d camera, vec3d light, vec3d direction, float* distance);
vec3d atom_normal(atom_t* atom, vec3d point);

void destroy_atom(atom_t* atom);
void print_atom(atom_t* atom);

// circle atom helpers
atom_t* create_sphere(vec3d position, float radius);
bool get_sphere(atom_t* atom, sphere_t* properties);

// plane atom helpers
atom_t* create_plane(vec3d position, vec3d normal);
bool get_plane(atom_t* atom, plane_t* properties);

// cylinder atom helpers
atom_t* create_cylinder(vec3d start, vec3d end, float radius);
bool get_cylinder(atom_t* atom, cylinder_t* properties);

#endif
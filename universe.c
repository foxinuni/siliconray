#include "universe.h"
#include "math.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

bool raycast_atom(atom_t *atom, vec3d origin, vec3d direction, float *distance) {
    bool match = false;

    switch (atom->kind) {
        case SPHERE: {
            sphere_t properties;
            if ((match = get_sphere(atom, &properties))) {
                vec3d oc = vec3d_sub(origin, properties.position);
                float a = vec3d_dot(direction, direction);
                float b = 2.0 * vec3d_dot(oc, direction);
                float c = vec3d_dot(oc, oc) - properties.radius * properties.radius;
                float discriminant = b * b - 4 * a * c;

                if (discriminant > 0) {
                    float temp = (-b - sqrt(discriminant)) / (2.0 * a);
                    if (temp < *distance && temp > 0) {
                        *distance = temp;
                    }
                }
            }
            break;
        }
        case PLANE: {
            plane_t properties;
            if ((match = get_plane(atom, &properties))) {
                float denominator = vec3d_dot(properties.normal, direction);
                if (denominator > 1e-6) {
                    vec3d p0l0 = vec3d_sub(properties.position, origin);
                    float t = vec3d_dot(p0l0, properties.normal) / denominator;
                    if (t >= 0 && t < *distance) {
                        *distance = t;
                    }
                }
            }
            break;
        }
        default:
            printf("Error: atom kind (%d) doesn't implement raycast!\n", atom->kind);
    }

    if (!match) {
        printf("Error: atom kind (%d) didn't match properties!\n", atom->kind);
    }

    return match;
}

vec3d raycast_universe(atom_t* atoms[], int count, vec3d origin, vec3d direction, float* distance) {
    float max_distance = *distance;
    float min_distance = *distance;
    vec3d hit_color = vec3d_new(0, 0, 0);

    for (int i = 0; i < count; i++) {
        if (raycast_atom(atoms[i], origin, direction, distance) && *distance < min_distance) {
            min_distance = *distance;
            
            // translate the distance to a color where 0 -> black and 1 -> white (full color spectrum)
            float t = min_distance / max_distance;

            hit_color.r = (1.0 - t) * 255;
            hit_color.g = t * 255;
            hit_color.b = (1.0 - fabs(0.5 - t) * 2.0) * 255;
        }
    }

    return hit_color;
}

void destroy_atom(atom_t* atom) {
    free(atom->properties);
    free(atom);
}

void print_atom(atom_t* atom) {
    bool match = false;

    switch (atom->kind) {
        case SPHERE: {
            sphere_t properties;
            if ((match = get_sphere(atom, &properties))) {
                printf("Sphere at (%.2f, %.2f, %.2f) with radius %.2f\n",
                    properties.position.x, properties.position.y, properties.position.z,
                    properties.radius);
            }
            break;
        }
        case PLANE: {
            plane_t properties;
            if ((match = get_plane(atom, &properties))) {
                printf("Plane at (%.2f, %.2f, %.2f) with normal (%.2f, %.2f, %.2f)\n",
                    properties.position.x, properties.position.y, properties.position.z,
                    properties.normal.x, properties.normal.y, properties.normal.z);
            }
            break;
        }
        default: 
            printf("Error: atom kind (%d) doesn't implement print!\n", atom->kind);
    }

    if (!match) {
        printf("Error: atom kind (%d) didn't match properties!\n", atom->kind);
    }
}

atom_t* create_sphere(vec3d position, float radius) {
    sphere_t* properties = malloc(sizeof(sphere_t));
    properties->position = position;
    properties->radius = radius;

    atom_t* atom = malloc(sizeof(atom_t));
    atom->kind = SPHERE;
    atom->properties = properties;

    return atom;
}

bool get_sphere(atom_t* atom, sphere_t* properties) {
    if (atom->kind != SPHERE) {
        return false;
    }

    *properties = *(sphere_t*)atom->properties;
    return true;
}

atom_t* create_plane(vec3d position, vec3d normal) {
    plane_t* properties = malloc(sizeof(plane_t));
    properties->position = position;
    properties->normal = normal;

    atom_t* atom = malloc(sizeof(atom_t));
    atom->kind = PLANE;
    atom->properties = properties;

    return atom;
}

bool get_plane(atom_t* atom, plane_t* properties) {
    if (atom->kind != PLANE) {
        return false;
    }

    *properties = *(plane_t*)atom->properties;
    return true;
}
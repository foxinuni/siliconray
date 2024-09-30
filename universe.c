#include "universe.h"
#include "math.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// a bit to large to be inline with raycast_atom()
bool raycast_cylinder(cylinder_t* cylinder, vec3d origin, vec3d direction, float* distance) {
    vec3d d = vec3d_sub(cylinder->end, cylinder->start); // Cylinder axis vector
    vec3d m = vec3d_sub(origin, cylinder->start);
    vec3d n = vec3d_sub(origin, cylinder->end);

    float md = vec3d_dot(m, d);
    float nd = vec3d_dot(n, d);
    float dd = vec3d_dot(d, d);

    float radius2 = cylinder->radius * cylinder->radius;

    // Coefficients for the quadratic equation
    float a = dd - vec3d_dot(direction, d) * vec3d_dot(direction, d);
    float b = dd * vec3d_dot(m, direction) - md * vec3d_dot(direction, d);
    float c = dd * vec3d_dot(m, m) - md * md - radius2 * dd;

    // Solve the quadratic equation
    float discriminant = b * b - a * c;
    if (discriminant < 0.0f) {
        return false; // No intersection
    }

    float sqrt_discriminant = sqrt(discriminant);
    float t0 = (-b - sqrt_discriminant) / a;
    float t1 = (-b + sqrt_discriminant) / a;

    if (t0 > t1) {
        float temp = t0;
        t0 = t1;
        t1 = temp;
    }

    // Check if the intersection points are within the bounds of the cylinder
    if (t0 < 0.0f) {
        t0 = t1; // If t0 is negative, use t1 instead
        if (t0 < 0.0f) {
            return false; // Both t0 and t1 are negative
        }
    }

    vec3d hit_point = vec3d_add(origin, vec3d_mul(direction, t0));
    vec3d hit_to_start = vec3d_sub(hit_point, cylinder->start);
    vec3d hit_to_end = vec3d_sub(hit_point, cylinder->end);

    if (vec3d_dot(hit_to_start, d) >= 0 && vec3d_dot(hit_to_end, d) <= 0) {
        *distance = t0;
        return true;
    }

    return false;
}

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
        case CYLINDER: {
            cylinder_t properties;
            if ((match = get_cylinder(atom, &properties))) {
                raycast_cylinder(&properties, origin, direction, distance);
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

vec3d raycast_universe(atom_t* atoms[], int count, vec3d camera, vec3d direction, float* distance) {
    float max_distance = *distance;
    float min_distance = *distance;
    vec3d hit_color = vec3d_new(0, 0, 0);

    vec3d light_position = vec3d_new(10, 10, -15);
    vec3d light_color = vec3d_new(255, 255, 255);

    for (int i = 0; i < count; i++) {
        if (raycast_atom(atoms[i], camera, direction, distance) && *distance < min_distance) {
            min_distance = *distance;
            
            // calculate the hit point and normal
            vec3d hit_point = vec3d_add(camera, vec3d_mul(direction, min_distance));
            vec3d normal = atom_normal(atoms[i], hit_point);

            // calculate the light direction and intensity
            vec3d light_direction = vec3d_norm(vec3d_sub(light_position, hit_point));

            // calculate the lambertian shading
            float lambertian = fmax(0, vec3d_dot(normal, light_direction));
            
            // translate the distance to a color where 0 -> black and 1 -> white (full color spectrum)
            float t = min_distance / max_distance;

            hit_color.r = (1.0 - t) * 255 * lambertian;
            hit_color.g = t * 255 * lambertian;
            hit_color.b = (1.0 - fabs(0.5 - t) * 2.0) * 255 * lambertian;
        }
    }

    return hit_color;
}

vec3d atom_normal(atom_t* atom, vec3d point) {
    vec3d normal = vec3d_new(0, 0, 0);

    switch (atom->kind) {
        case SPHERE: {
            sphere_t properties;
            if (get_sphere(atom, &properties)) {
                normal = vec3d_norm(vec3d_sub(point, properties.position));
            }
            break;
        }
        case PLANE: {
            plane_t properties;
            if (get_plane(atom, &properties)) {
                normal = properties.normal;
            }
            break;
        }
        case CYLINDER: {
            cylinder_t properties;
            if (get_cylinder(atom, &properties)) {
                vec3d oc = vec3d_sub(point, properties.start);
                vec3d d = vec3d_sub(properties.end, properties.start);
                float t = vec3d_dot(oc, d) / vec3d_dot(d, d);
                vec3d p = vec3d_add(properties.start, vec3d_mul(d, t));
                normal = vec3d_norm(vec3d_sub(point, p));
            }
            break;
        }
        default:
            printf("Error: atom kind (%d) doesn't implement normal!\n", atom->kind);
    }

    return normal;
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
        case CYLINDER: {
            cylinder_t properties;
            if ((match = get_cylinder(atom, &properties))) {
                printf("Cylinder from (%.2f, %.2f, %.2f) to (%.2f, %.2f, %.2f) with radius %.2f\n",
                    properties.start.x, properties.start.y, properties.start.z,
                    properties.end.x, properties.end.y, properties.end.z,
                    properties.radius);
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

atom_t* create_cylinder(vec3d start, vec3d end, float radius) {
    cylinder_t* cylinder = (cylinder_t*)malloc(sizeof(cylinder_t));
    cylinder->start = start;
    cylinder->end = end;
    cylinder->radius = radius;

    atom_t* atom = (atom_t*)malloc(sizeof(atom_t));
    atom->kind = CYLINDER;
    atom->properties = (void*)cylinder;
    return atom;
}

bool get_cylinder(atom_t* atom, cylinder_t* properties) {
    if (atom->kind != CYLINDER) {
        return false;
    }

    *properties = *(cylinder_t*)atom->properties;
    return true;
}
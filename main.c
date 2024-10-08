#include <stb/stb_image_write.h>
#include <stdio.h>
#include <pthread.h>

#include "math.h"
#include "rendering.h"
#include "universe.h"

#ifndef WIDTH
#define WIDTH 1000
#endif

#ifndef HEIGHT
#define HEIGHT 1000
#endif

#ifndef FRAMES
#define FRAMES 100
#endif

#if FRAMES < 1
#error "FRAMES must be greater than 0!"
#endif

#ifndef RINGS
#define RINGS 0
#endif

void save_png(image_t* image, int frame) {
    static int count = 0;
    static pthread_mutex_t mutex;

    char filename[10];
    snprintf(filename, 9, "%d.png", frame);
    stbi_write_png(filename, image->width, image->height, 3, image->image, 3 * image->width);

    pthread_mutex_lock(&mutex);
    fprintf(stderr, "\rrendered frame: %d/%d", count++, FRAMES);
    pthread_mutex_unlock(&mutex);
}

int main(int argc, char* argv[]){
    // create atoms
    atom_t* atoms[] = {
        // vertices
        create_sphere(vec3d_new(5, 5, 5), 1),
        create_sphere(vec3d_new(-5, 5, 5), 1),
        create_sphere(vec3d_new(-5, -5, 5), 1),
        create_sphere(vec3d_new(5, -5, 5), 1),
        create_sphere(vec3d_new(5, 5, -5), 1),
        create_sphere(vec3d_new(-5, 5, -5), 1),
        create_sphere(vec3d_new(-5, -5, -5), 1),
        create_sphere(vec3d_new(5, -5, -5), 1),

        // faces
        create_sphere(vec3d_new(0, 0, 5), 1),
        create_sphere(vec3d_new(0, 0, -5), 1),
        create_sphere(vec3d_new(0, 5, 0), 1),
        create_sphere(vec3d_new(0, -5, 0), 1),
        create_sphere(vec3d_new(5, 0, 0), 1),
        create_sphere(vec3d_new(-5, 0, 0), 1),

        // quirkies
        create_sphere(vec3d_new(-2.5, 2.5, 2.5), 1),
        create_sphere(vec3d_new(2.5, -2.5, 2.5), 1),
        create_sphere(vec3d_new(2.5, 2.5, -2.5), 1),
        create_sphere(vec3d_new(-2.5, -2.5, -2.5), 1),

        // bottom connections
        create_cylinder(vec3d_new(-5, -5, -5), vec3d_new(-2.5, -2.5, -2.5), 0.2), // vert - quirky
        create_cylinder(vec3d_new(-2.5, -2.5, -2.5), vec3d_new(0, -5, 0), 0.2),   // quirky - face
        create_cylinder(vec3d_new(0, -5, 0), vec3d_new(2.5, -2.5, 2.5), 0.2),     // face - quirky
        create_cylinder(vec3d_new(2.5, -2.5, 2.5), vec3d_new(5, -5, 5), 0.2),     // quirky - vert

        // half-bottom connections
        create_cylinder(vec3d_new(-5, 0, 0), vec3d_new(-2.5, -2.5, -2.5), 0.2), // face - bottom quirky
        create_cylinder(vec3d_new(-2.5, -2.5, -2.5), vec3d_new(0, 0, -5), 0.2), // bottom quirky - face
        create_cylinder(vec3d_new(5, 0, 0), vec3d_new(2.5, -2.5, 2.5), 0.2),    // face - bottom quirky
        create_cylinder(vec3d_new(2.5, -2.5, 2.5), vec3d_new(0, 0, 5), 0.2),    // bottom quirky - face

        // top connections
        create_cylinder(vec3d_new(-5, 5, 5), vec3d_new(-2.5, 2.5, 2.5), 0.2), // vert - quirky
        create_cylinder(vec3d_new(-2.5, 2.5, 2.5), vec3d_new(0, 5, 0), 0.2),   // quirky - face
        create_cylinder(vec3d_new(0, 5, 0), vec3d_new(2.5, 2.5, -2.5), 0.2),   // face - quirky
        create_cylinder(vec3d_new(2.5, 2.5, -2.5), vec3d_new(5, 5, -5), 0.2),   // quirky - vert

        // half-top connections
        create_cylinder(vec3d_new(-5, 0, 0), vec3d_new(-2.5, 2.5, 2.5), 0.2), // face - top quirky
        create_cylinder(vec3d_new(-2.5, 2.5, 2.5), vec3d_new(0, 0, 5), 0.2), // top quirky - face
        create_cylinder(vec3d_new(5, 0, 0), vec3d_new(2.5, 2.5, -2.5), 0.2),  // face - top quirky
        create_cylinder(vec3d_new(2.5, 2.5, -2.5), vec3d_new(0, 0, -5), 0.2), // top quirky - face
    };

    if (generate_frames(atoms, 34, WIDTH, HEIGHT, save_png, FRAMES)) {
        fprintf(stderr, "\rsuccessfully rendered %d frames!\n", FRAMES);
    } else {
        fprintf(stderr, "\rfailed to render frames!\n");
    }

    // destroy atom
    for (int i = 0; i < 1; i++) {
        destroy_atom(atoms[i]);
    }
}

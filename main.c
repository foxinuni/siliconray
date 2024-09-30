#include <stb/stb_image_write.h>

#include "math.h"
#include "universe.h"

#ifndef DISTANCE
#define DISTANCE 50
#endif

#ifndef WIDTH
#define WIDTH 5000
#endif

#ifndef HEIGHT
#define HEIGHT 5000
#endif

int main(int argc, char* argv[]){
    // allocate image
    unsigned char* image = (unsigned char*) malloc(WIDTH * HEIGHT * 3);
    
    // create origin
    vec3d camera = vec3d_new(0, 0, -12.5);

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

    // raycast atom for each pixel    
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            // transform pixel to NDC
            float ndc_x = (2 * (x + 0.5) / WIDTH - 1) * WIDTH / HEIGHT;
            float ndc_y = 1 - 2 * (y + 0.5) / HEIGHT;

            // set distance
            float distance = DISTANCE;

            // raycast atom
            vec3d hit_color = raycast_universe(atoms, 34, camera, vec3d_norm(vec3d_new(ndc_x, ndc_y, 1)), &distance);

            // write color
            image[3 * (y * WIDTH + x) + 0] = hit_color.r;
            image[3 * (y * WIDTH + x) + 1] = hit_color.g;
            image[3 * (y * WIDTH + x) + 2] = hit_color.b;
        }
    }

    // write image
    printf("Writing image...\n");
    stbi_write_png("output.png", WIDTH, HEIGHT, 3, image, 3 * WIDTH);

    // destroy atom
    for (int i = 0; i < 1; i++) {
        destroy_atom(atoms[i]);
    }
}

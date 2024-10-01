#include <stb/stb_image_write.h>
#include <stdio.h>

#include "math.h"
#include "universe.h"

#ifndef DISTANCE
#define DISTANCE 50
#endif

#ifndef WIDTH
#define WIDTH 1000
#endif

#ifndef HEIGHT
#define HEIGHT 1000
#endif

#ifndef FRAMES
#define FRAMES 100
#endif

#define PI 3.14159265359
#define FOV 90

int main(int argc, char* argv[]){
    // allocate image
    unsigned char* image = (unsigned char*) malloc(WIDTH * HEIGHT * 3);

    // basic vectors
    vec3d look_at = vec3d_new(0, 0, 0);
    vec3d up = vec3d_new(0, 1, 0);

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

    // generate frames
    if (FRAMES < 1) {
        printf("Error: FRAMES must be greater than 0!\n");
        return 1;
    }

    float angleSkip = 2.0 * PI / (float) FRAMES;
    for (float i = 0, c = 0; i < 2 * PI; i += angleSkip, c++) {
        fprintf(stderr, "\rRendering frame %d/%d", (int) c, FRAMES);

        // rotate camera & light
        vec3d camera = vec3d_new(13 * sin(i), 0, -13 * cos(i));
        vec3d light = vec3d_new(10 * sin(i), 10, -15 * cos(i));

        // calculate forward vector
        vec3d forward = vec3d_norm(vec3d_sub(look_at, camera)); // from the camera to the look_at point
        vec3d horizontal = vec3d_norm(vec3d_cross(forward, up)); // right vector
        vec3d vertical = vec3d_cross(horizontal, forward); // vertical vector

        // raycast atom for each pixel    
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                // set distance
                float distance = DISTANCE;

                // calculate direction
                vec2d perspective = vec2d_new(
                    2.0 * (x + 0.5) / (float) WIDTH - 1.0,
                    1 - 2.0 * (y + 0.5) / (float) HEIGHT
                );

                // transform perspective based on FOV
                perspective.x *= tan(0.5 * FOV * PI / 180.0) * ((float) WIDTH / (float) HEIGHT);
                perspective.y *= tan(0.5 * FOV * PI / 180.0);

                // calculate direction
                vec3d direction = vec3d_norm(vec3d_add(
                    vec3d_add(
                        vec3d_mul(horizontal, perspective.x),
                        vec3d_mul(vertical, perspective.y)
                    ),
                    forward
                ));

                // raycast atom
                vec3d hit_color = raycast_universe(atoms, 34, camera, light, direction, &distance);

                // write color
                image[3 * (y * WIDTH + x) + 0] = hit_color.r;
                image[3 * (y * WIDTH + x) + 1] = hit_color.g;
                image[3 * (y * WIDTH + x) + 2] = hit_color.b;
            }
        }

        // write image
        char filename[10];
        snprintf(filename, 9, "%d.png", (int) c);
        stbi_write_png(filename, WIDTH, HEIGHT, 3, image, 3 * WIDTH);
    }

    fprintf(stderr, "\n");

    // destroy atom
    for (int i = 0; i < 1; i++) {
        destroy_atom(atoms[i]);
    }
}

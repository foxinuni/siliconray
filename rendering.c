#include "rendering.h"
#include "math.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

bool generate_frame(image_t* image, raycast_t* raycast, atom_t** universe, int atoms) {
    // basic vectors
    const vec3d up = vec3d_new(0, 1, 0);

    // calculate forward vector
    vec3d forward = vec3d_norm(vec3d_sub(raycast->target, raycast->camera)); // from the camera to the look_at point
    vec3d horizontal = vec3d_norm(vec3d_cross(forward, up)); // right vector
    vec3d vertical = vec3d_cross(horizontal, forward); // vertical vector

    // raycast atom for each pixel    
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            // set distance
            float distance = raycast->distance;

            // calculate perspective
            vec2d perspective = vec2d_new(
                2.0 * (x + 0.5) / (float) image->width - 1.0,
                1 - 2.0 * (y + 0.5) / (float) image->height
            );

            // transform perspective based on FOV
            perspective.x *= tan(0.5 * FOV * PI / 180.0) * ((float) image->width / (float) image->height);
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
            vec3d hit_color = raycast_universe(universe, atoms, raycast->camera, raycast->light, direction, &distance);

            // write color
            image->image[3 * (y * image->width + x) + 0] = hit_color.r;
            image->image[3 * (y * image->width + x) + 1] = hit_color.g;
            image->image[3 * (y * image->width + x) + 2] = hit_color.b;
        }
    }

    return true;
}

void* gen_frames_worker(void* thread_info) {
    // translate
    frame_gen_t* info = (frame_gen_t*) thread_info;

    // generate image
    image_t image = {
        .height = info->height,
        .width = info->width,
        .image = (unsigned char*) malloc(info->height * info->width * 3),
    };

    // calculate angle skip
    float angle_skip = 2.0 * PI / (float) info->frames;

    // calculate angles
    float from_angle = info->from * angle_skip;
    float to_angle = info->to * angle_skip;

    int count = 0;
    for (float angle = from_angle; angle < to_angle; angle += angle_skip) {
        // rotate camera & light
        vec3d camera = vec3d_new(13 * sin(angle), 0, -13 * cos(angle));
        vec3d light = vec3d_new(10 * sin(angle), 10, -15 * cos(angle));

        // create raycast
        raycast_t raycast = {
            .camera = camera,
            .light = light,
            .target = vec3d_new(0, 0, 0),
            .distance = 50
        };

        // generate frame
        if (!generate_frame(&image, &raycast, info->universe, info->atoms)) {
            fprintf(stderr, "error: failed to generate frame %d!\n", info->from + count);
            return (void*) false;
        }

        // call callback
        info->callback(&image, info->from + count);
        count++;
    }

    // free image
    free(image.image);

    return (void*) true;
}

bool generate_frames(atom_t** universe, int atoms, unsigned int width, unsigned int height, frame_cb callback, int frames) {
    if (frames < 1) {
        fprintf(stderr, "error: frames must be greater than 0!\n");
        return false;
    }

    if (frames % THREADS != 0) {
        fprintf(stderr, "error: frames must be divisible by THREADS!\n");
        return false;
    }	

    // calculate threads frames
    int frames_per_thread = frames / THREADS;
    fprintf(stderr, "rendering %d frames with %d threads (%d frames per thread)...\n", frames, THREADS, frames_per_thread);
    
    // begin threads
    frame_gen_t info[THREADS];
    pthread_t threads[THREADS];

    for (int i = 0; i < THREADS; i++) {
        info[i] = (frame_gen_t) {
            .universe = universe,
            .callback = callback,
            .width = width,
            .height = height,
            .atoms = atoms,
            .frames = frames,
            .from = i * frames_per_thread,
            .to = (i + 1) * frames_per_thread
        };

        fprintf(stderr, "thread %d -> (from: %d, to: %d) [%p]\n", i, info[i].from, info[i].to, &info[i]);

        if (pthread_create(&threads[i], NULL, gen_frames_worker, &info[i])) {
            fprintf(stderr, "error: failed to create thread %d!\n", i);
            return false;
        }
    }

    // join threads
    bool success = true;
    for (int i = 0; i < THREADS; i++) {
        if (pthread_join(threads[i],  (void**) &success)) {
            fprintf(stderr, "error: failed to join thread %d!\n", i);
            return false;
        }

        if (!success) {
            fprintf(stderr, "error: worker failed to render frames!\n");
            return false;
        }
    }

    return true;	
}
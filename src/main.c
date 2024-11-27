#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "triangle.h"


triangle_t triangles_to_render[N_MESH_FACES];

const int N_POINTS = 9*9*9;
vec3_t cube_points[N_POINTS];
vec2_t projected_points[N_POINTS];

vec3_t camera_position = {
    .x = 0,
    .y = 0,
    .z = -5
};

vec3_t cube_rotation = {
    .x = 0,
    .y = 0,
    .z = 0
};

float fov_factor = 640;

bool is_running = false;
int previous_frame_time = 0;


void setup(void) {
    color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height);
    if(!color_buffer){
        printf("ERROR: allocating buffer \n");
        is_running = false;
        return;
    }

    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );

}


vec2_t ortho_project(vec3_t point){
    vec2_t projected_point = {
        .x = (fov_factor * point.x),
        .y = (fov_factor * point.y)
    };

    return projected_point;
}

vec2_t persp_project(vec3_t point){
    vec2_t projected_point = {
        .x = (fov_factor * point.x) / point.z,
        .y = (fov_factor * point.y) / point.z
    };

    return projected_point;
}

void process_input(void){
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type)
    {
    case SDL_QUIT: // close x button
        is_running = false;
        break;
    case SDL_KEYDOWN:
    if(event.key.keysym.sym == SDLK_ESCAPE){
        is_running = false;
    }
    default:
        break;
    }
}

void update(void) {
    // block until the consistent interval has passed
    //while(!SDL_TICKS_PASSED(SDL_GetTicks(), previous_frame_time + FRAME_TARGET_TIME));
    
    // delay exec until enough time passed
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
    if(time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME){
        SDL_Delay(time_to_wait);
    }

    previous_frame_time = SDL_GetTicks();

    cube_rotation.x += 0.01;
    cube_rotation.y += 0.01;
    cube_rotation.z += 0.01;

    for(int i=0; i < N_MESH_FACES; i++){
        face_t mesh_face = mesh_faces[i];
        vec3_t  face_verts[3];
        face_verts[0] = mesh_verticies[mesh_face.a -1];
        face_verts[1] = mesh_verticies[mesh_face.b -1];
        face_verts[2] = mesh_verticies[mesh_face.c -1];
        
        triangle_t projected_triangle;

        for(int j =0; j < 3; j++){
            vec3_t transformed_vert = face_verts[j];
            transformed_vert = vec3_rotate_x(transformed_vert, cube_rotation.x);
            transformed_vert = vec3_rotate_y(transformed_vert, cube_rotation.y);
            transformed_vert = vec3_rotate_z(transformed_vert, cube_rotation.z);

            // translate verts back in z from the camera
            transformed_vert.z -= camera_position.z;

            vec2_t projected_point = persp_project(transformed_vert);
            //vec2_t projected_point = ortho_project(transformed_vert);

            // scale and translate to middle of screen
            projected_point.x += (window_width * 0.5);
            projected_point.y += (window_height * 0.5);

            projected_triangle.points[j] = projected_point;
            
        }

        triangles_to_render[i] = projected_triangle;
    }
}


void render(void) {
    //SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // rgba
    //SDL_RenderClear(renderer);

    //draw_grid();
    //draw_rect(300,200, 300, 150, 0xFFFF00FF);

    for(int i=0; i < N_MESH_FACES; i++){
        triangle_t triangle = triangles_to_render[i];

        // draw verts
        draw_rect(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFF00FF);
        draw_rect(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFF00FF);
        draw_rect(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFF00FF);

        // draw edges
        draw_triangle(
            triangle.points[0].x,
            triangle.points[0].y,
            triangle.points[1].x,
            triangle.points[1].y,
            triangle.points[2].x,
            triangle.points[2].y,
            0xFFFF00FF
        );
    }

    render_color_buffer();
    clear_color_buffer(0xFFFFFF00);

    SDL_RenderPresent(renderer);
}



int main(void){
    printf("initializing \n");

    is_running = initialize_window();

    setup();

    while(is_running){
        process_input();
        update();
        render();
    }
    destroy_window();

    return 0;
}

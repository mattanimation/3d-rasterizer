#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include "array.h"
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "triangle.h"


// dynamic array of triangles to render
triangle_t* triangles_to_render = NULL;

const int N_POINTS = 9*9*9;
vec3_t cube_points[N_POINTS];
vec2_t projected_points[N_POINTS];

vec3_t camera_position = { 0, 0, 0 };


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

    // init the mesh
    //load_cube_mesh_data();
    load_obj_file_data("./assets/suzanne.obj");


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

    // init the tri arr
    triangles_to_render = NULL;

    //mesh.rotation.x += 0.01;
    mesh.rotation.y += 0.01;
    // mesh.rotation.z += 0.01;

    int num_faces = array_length(mesh.faces);
    for(int i=0; i < num_faces; i++){
        face_t mesh_face = mesh.faces[i];
        vec3_t  face_verts[3];
        face_verts[0] = mesh.verticies[mesh_face.a -1];
        face_verts[1] = mesh.verticies[mesh_face.b -1];
        face_verts[2] = mesh.verticies[mesh_face.c -1];
        
        triangle_t projected_triangle;

        vec3_t transformed_verticies[3];

        for(int j =0; j < 3; j++){
            vec3_t transformed_vert = face_verts[j];
            transformed_vert = vec3_rotate_x(transformed_vert, mesh.rotation.x);
            transformed_vert = vec3_rotate_y(transformed_vert, mesh.rotation.y);
            transformed_vert = vec3_rotate_z(transformed_vert, mesh.rotation.z);

            // translate verts back in z from the camera
            transformed_vert.z += 5;

            // store for culling and projection
            transformed_verticies[j] = transformed_vert;
        }

        // backface culling
        vec3_t va = transformed_verticies[0]; /*   A    */
        vec3_t vb = transformed_verticies[1]; /*  / \   */
        vec3_t vc = transformed_verticies[2]; /* C---B  */
        // grab the 2 vectors from a
        vec3_t v_ab = vec3_sub(vb, va);
        vec3_t v_ac = vec3_sub(vc, va);
        vec3_normalize_fast(&v_ab);
        vec3_normalize_fast(&v_ac);
        // get the normal - order is based on clickwise winding of face
        // but handedness determines order to get normal in the right direction
        // this is left handed
        vec3_t normal = vec3_cross(v_ab, v_ac);
        // normlaize it
        //vec3_normalize(&normal);
        vec3_normalize_fast(&normal);

        // get vec to camera
        vec3_t camera_ray = vec3_sub(camera_position, va);
        // get the dot to the camera
        float dot_normal_cam = vec3_dot(normal, camera_ray);
        // see how aligned the face is to the camera dot
        // 0 is perpendicular, 1.0 is aligned, -1.0 is opposite
        // dot_normal_cam > 0) is inverted version
        if(dot_normal_cam < 0){
            // cull it
            continue;
        }


        // do projection of verts for the face
        for(int j =0; j < 3; j++){
            vec2_t projected_point = persp_project(transformed_verticies[j]);
            //vec2_t projected_point = ortho_project(transformed_vert);

            // scale and translate to middle of screen
            projected_point.x += (window_width * 0.5);
            projected_point.y += (window_height * 0.5);

            projected_triangle.points[j] = projected_point;
            
        }

        array_push(triangles_to_render, projected_triangle);
        //triangles_to_render[i] = projected_triangle;
    }
}


void render(void) {
    
    draw_grid();

    int num_triangles = array_length(triangles_to_render);

    for(int i=0; i < num_triangles; i++){
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

    //clear tri arr
    array_free(triangles_to_render);

    render_color_buffer();
    clear_color_buffer(0xFFFFFF00);

    SDL_RenderPresent(renderer);
}

// free up all memory
void free_resources(void){
    printf("cleaning up \n");
    array_free(mesh.verticies);
    array_free(mesh.faces);
    free(color_buffer);
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
    free_resources();

    return 0;
}

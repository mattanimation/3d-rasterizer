#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include "array.h"
#include "display.h"
#include "light.h"
#include "matrix.h"
#include "mesh.h"
#include "texture.h"
#include "triangle.h"
#include "vector.h"
#include "upng.h"

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif


// dynamic array of triangles to render
triangle_t* triangles_to_render = NULL;

vec3_t camera_position = { 0, 0, 0 };
mat4_t proj_matrix;

bool is_running = false;
int previous_frame_time = 0;


void arr_swap(triangle_t* arr, int i, int j){
    triangle_t temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
}

// deprecated with z-buffer update
// void bubbleSort(triangle_t arr[], int n){
//     for(int i=0; i < n -1; i++){

//         bool swapped = false;
        
//         for(int j=0; j < n - i - 1; j++){
//             if(arr[j].avg_depth > arr[j+1].avg_depth){
//                 arr_swap(arr, j, j+1);
//                 swapped = true;
//             }
//         }
        
//         if(swapped == false){
//             break;
//         }
//     }
// }


void setup(void) {

    // Pressing “1” displays the wireframe and a small red dot for each triangle vertex
    // Pressing “2” displays only the wireframe lines
    // Pressing “3” displays filled triangles with a solid color
    // Pressing “4” displays both filled triangles and wireframe lines
    // Pressing “c” we should enable back-face culling
    // Pressing “d” we should disable the back-face culling
    render_method = RENDER_WIRE;
    cull_method = CULL_BACKFACE;

    color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height);
    if(!color_buffer){
        printf("ERROR: allocating color buffer \n");
        is_running = false;
        return;
    }

    z_buffer = (float*) malloc(sizeof(float) * window_width * window_height);
    if(!z_buffer){
        printf("ERROR: allocating z buffer \n");
        is_running = false;
        return;
    }

    color_buffer_texture = SDL_CreateTexture(
        renderer,
        // SDL_PIXELFORMAT_ARGB8888,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );

    // init perspective projection matrix
    float fov = M_PI / 3.0;   // same as 180/3 or 60 degress;
    float aspect = (float)window_height / (float)window_width;
    float znear = 0.1;
    float zfar = 100.0;
    proj_matrix = mat4_make_perspective(fov, aspect, znear, zfar);

    // manually load tex data
    // mesh_texture = (uint32_t*)REDBRICK_TEXTURE;
    // texture_width = 64;
    // texture_height = 64;

    // init the mesh
    //load_cube_mesh_data();
    //load_obj_file_data("./assets/suzanne.obj");
    //load_obj_file_data("./assets/cube.obj");
    //load_obj_file_data("./assets/f22.obj");
    load_obj_file_data("./assets/crab.obj");
    //load_obj_file_data("./assets/drone.obj");
    //load_obj_file_data("./assets/efa.obj");
    //load_obj_file_data("./assets/f117.obj");
    //load_obj_file_data("./assets/pikuma.obj");

    //load_png_texture_data("./assets/cube.png");
    //load_png_texture_data("./assets/f22.png");
    load_png_texture_data("./assets/crab.png");
    //load_png_texture_data("./assets/drone.png");
    //load_png_texture_data("./assets/efa.png");
    //load_png_texture_data("./assets/f117.png");
    //load_png_texture_data("./assets/pikuma.png");



}


// vec2_t ortho_project(vec3_t point){
//     vec2_t projected_point = {
//         .x = (fov_factor * point.x),
//         .y = (fov_factor * point.y)
//     };

//     return projected_point;
// }

// vec2_t persp_project(vec3_t point){
//     vec2_t projected_point = {
//         .x = (fov_factor * point.x) / point.z,
//         .y = (fov_factor * point.y) / point.z
//     };

//     return projected_point;
// }

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
    if(event.key.keysym.sym == SDLK_1){
        render_method = RENDER_WIRE_VERTEX;
    }
    if(event.key.keysym.sym == SDLK_2){
        render_method = RENDER_WIRE;
    }
    if(event.key.keysym.sym == SDLK_3){
        render_method = RENDER_FILL_TRIANGLE;
    }
    if(event.key.keysym.sym == SDLK_4){
        render_method = RENDER_FILL_TRIANGLE_WIRE;
    }
    if(event.key.keysym.sym == SDLK_5){
        render_method = RENDER_TEXTURED;
    }
    if(event.key.keysym.sym == SDLK_6){
        render_method = RENDER_TEXTURED_WIRE;
    }
    if(event.key.keysym.sym == SDLK_c){
        cull_method = CULL_BACKFACE;
    }
    if(event.key.keysym.sym == SDLK_d){
        cull_method = CULL_NONE;
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

    // change values per frame
    //mesh.rotation.x += 0.02;
    mesh.rotation.y += 0.04;
    //mesh.rotation.z += 0.006;
    //mesh.scale.x += 0.002;
    //mesh.scale.y += 0.002;
    //mesh.scale.z += 0.002;
    //mesh.translation.x += 0.01;
    // move back 5 units from camera
    mesh.translation.z = 5.0;
    

    // create a scale matrix that will be used to mult the scale
    mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    // create a translation matrix to move the mesh
    mat4_t translation_matrix = mat4_make_translation(
        mesh.translation.x,
        mesh.translation.y,
        mesh.translation.z
    );
    mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
    mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
    mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);

    int num_faces = array_length(mesh.faces);
    for(int i=0; i < num_faces; i++){
        face_t mesh_face = mesh.faces[i];
        vec3_t  face_verts[3];
        face_verts[0] = mesh.verticies[mesh_face.a];
        face_verts[1] = mesh.verticies[mesh_face.b];
        face_verts[2] = mesh.verticies[mesh_face.c];
        

        vec4_t transformed_verticies[3];

        for(int j =0; j < 3; j++){
            vec4_t transformed_vert = vec4_from_vec3(face_verts[j]);

            // create a world matrix to combine, tx, rx, and sx
            mat4_t world_matrix = mat4_identity();
            // order matters, so world is 2nd item applies to -->
            // and order matters for the matrix so sx, rx, then tx since matrix math is not cumulative
            // [T]*[R]*[S]*v
            world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
            world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

            transformed_vert = mat4_mul_vec4(world_matrix, transformed_vert);

            // store for culling and projection
            transformed_verticies[j] = transformed_vert;
        }

        
        
        vec3_t va = vec3_from_vec4(transformed_verticies[0]); /*   A    */
        vec3_t vb = vec3_from_vec4(transformed_verticies[1]); /*  / \   */
        vec3_t vc = vec3_from_vec4(transformed_verticies[2]); /* C---B  */
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

        // backface culling
        if(cull_method == CULL_BACKFACE){
            // see how aligned the face is to the camera dot
            // 0 is perpendicular, 1.0 is aligned, -1.0 is opposite
            // dot_normal_cam > 0) is inverted version
            if(dot_normal_cam < 0){
                // cull it
                continue;
            }
        }

        vec4_t projected_points[3];

        // do projection of verts for the face
        for(int j =0; j < 3; j++){

            projected_points[j] = mat4_mul_vec4_project(proj_matrix, transformed_verticies[j]);
            //projected_points[j] = persp_project(vec3_from_vec4(transformed_verticies[j]));
            //vec2_t projected_point = ortho_project(transformed_vert);

            // scale into the view
            projected_points[j].x *= (window_width * 0.5);
            projected_points[j].y *= (window_height * 0.5);

            // invert the y values to account for flipped screen u coordinates
            projected_points[j].y *= -1;
            
            //translate to middle of screen
            projected_points[j].x += (window_width * 0.5);
            projected_points[j].y += (window_height * 0.5);

            
        }

        // need avg z value of verts in a face
        // NOTE: removed when changing to z-buffer
        //float avg_depth = (transformed_verticies[0].z + transformed_verticies[1].z + transformed_verticies[2].z) / 3;
        
        uint32_t triangle_color = mesh_face.color;

        // calc light intensity of the light hitting the face
        // get alignment of light -> normal angle
        // invert the value due to how global light angle works relative to camera origin
        float light_intensity_factor = -vec3_dot(light.direction, normal);
        // calc color of face from light
        uint32_t shaded_color = light_apply_intensity(triangle_color, light_intensity_factor);
        

        triangle_t projected_triangle = {
            .points = {
                { projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w},
                { projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w},
                { projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w},
            },
            .texcoords = {
                { mesh_face.a_uv.u, mesh_face.a_uv.v },
                { mesh_face.b_uv.u, mesh_face.b_uv.v },
                { mesh_face.c_uv.u, mesh_face.c_uv.v },
            },
            .color = shaded_color,
            //.avg_depth = avg_depth // removed when changing to z-buffer
        };

        array_push(triangles_to_render, projected_triangle);
        //triangles_to_render[i] = projected_triangle;
    }

    // sort triangles by avg_depth (painters algorithm)
    //bubbleSort(triangles_to_render, array_length(triangles_to_render));
}


void render(void) {
    
    draw_grid();

    int num_triangles = array_length(triangles_to_render);

    for(int i=0; i < num_triangles; i++){
        triangle_t triangle = triangles_to_render[i];

        // draw filled
        if(render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE){
            uint32_t color = 0xEEEEEEFF;
            if(triangle.color) {
                color = triangle.color;
            }
            draw_filled_triangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, // vert a
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, // vert b
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, // vert c
                color 
            );
        }

        //draw textured
        if(render_method == RENDER_TEXTURED || render_method == RENDER_TEXTURED_WIRE){
            // uint32_t color = 0xEEEEEEFF;
            // if(triangle.color) {
            //     color = triangle.color;
            // }
            
            draw_textured_triangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.texcoords[0].u, triangle.texcoords[0].v,
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.texcoords[1].u, triangle.texcoords[1].v,
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.texcoords[2].u, triangle.texcoords[2].v,
                mesh_texture
            );
        }

        // draw wire
        if(render_method == RENDER_WIRE || render_method == RENDER_WIRE_VERTEX || render_method == RENDER_FILL_TRIANGLE_WIRE || render_method == RENDER_TEXTURED_WIRE){
            uint32_t color = 0xFF7F00FF;
            // draw edges
            draw_triangle(
                triangle.points[0].x,
                triangle.points[0].y,
                triangle.points[1].x,
                triangle.points[1].y,
                triangle.points[2].x,
                triangle.points[2].y,
                color
            );
        }


        if(render_method == RENDER_WIRE_VERTEX){
            // draw verts
            draw_rect(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFF00FF);
            draw_rect(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFF00FF);
            draw_rect(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFF00FF);
        }

    }
    

    //clear tri arr
    array_free(triangles_to_render);

    render_color_buffer();
    clear_color_buffer(0xFFFFFF00);
    clear_z_buffer();

    SDL_RenderPresent(renderer);
}

// free up all memory
void free_resources(void){
    printf("cleaning up \n");
    array_free(mesh.verticies);
    array_free(mesh.faces);
    upng_free(png_texture);
    free(color_buffer);
    free(z_buffer);
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

#ifndef MESH_H
#define MESH_H

#include <stdio.h>
#include <string.h>
#include "vector.h"
#include "triangle.h"
#include "array.h"

#define N_CUBE_VERTICIES 8
#define N_CUBE_FACES (6 * 2) // 6 faces, 2 tris per face

extern vec3_t cube_verticies[N_CUBE_VERTICIES];
extern face_t cube_faces[N_CUBE_FACES];

// defines a struct for dynamic size meshes
typedef struct {
	vec3_t* verticies;   // dynamic array of verts
	face_t* faces;       // dynamic array of faces
	vec3_t rotation;     // x,y,z (euler angles)
	vec3_t scale;        // x,y,z scale
	vec3_t translation;  // x,y,z pos
} mesh_t;

extern mesh_t mesh;

void load_cube_mesh_data(void);
void load_obj_file_data(const char* path);


#endif
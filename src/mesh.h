#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"

#define N_MESH_VERTICIES 8
extern vec3_t mesh_verticies[N_MESH_VERTICIES];

#define N_MESH_FACES (6 * 2) // 6 faces, 2 tris per face
extern face_t mesh_faces[N_MESH_FACES];


#endif
#include "mesh.h"

mesh_t mesh = {
	.verticies = NULL,
	.faces = NULL,
	.rotation = {0, 0, 0},
	.scale = {1.0, 1.0, 1.0},
	.translation = {0, 0, 0}
};

vec3_t cube_verticies[N_CUBE_VERTICIES] = {
    {.x = -1, .y = -1, .z = -1}, //1
    {.x = -1, .y = 1, .z = -1}, //2
    {.x = 1, .y = 1, .z = -1}, //3
    {.x = 1, .y = -1, .z = -1}, //4
    {.x = 1, .y = 1, .z = 1}, //5
    {.x = 1, .y = -1, .z = 1}, //6
    {.x = -1, .y = 1, .z = 1}, //7
    {.x = -1, .y = -1, .z = 1}, //8
};

// colored
// face_t cube_faces[N_CUBE_FACES] = {
// 	//front
// 	{.a = 1, .b = 2, .c = 3, .color = 0xFFFF0000},
// 	{.a = 1, .b = 3, .c = 4, .color = 0xFFFF0000},
// 	//right
// 	{.a = 4, .b = 3, .c = 5, .color = 0xFF00FF00},
// 	{.a = 4, .b = 5, .c = 6, .color = 0xFF00FF00},
// 	//back
// 	{.a = 6, .b = 5, .c = 7, .color = 0xFF0000FF},
// 	{.a = 6, .b = 7, .c = 8, .color = 0xFF0000FF},
// 	//left
// 	{.a = 8, .b = 7, .c = 2, .color = 0xFF00FFFF},
// 	{.a = 8, .b = 2, .c = 1, .color = 0xFF00FFFF},
// 	//top
// 	{.a = 2, .b = 7, .c = 5, .color = 0xFFFFFFF0},
// 	{.a = 2, .b = 5, .c = 3, .color = 0xFFFFFFF0},
// 	//bottom
// 	{.a = 6, .b = 8, .c = 1, .color = 0xFFF0FF0F},
// 	{.a = 6, .b = 1, .c = 4, .color = 0xFFF0FF0F},
// };

// all white faces
// face_t cube_faces[N_CUBE_FACES] = {
// 	//front
// 	{.a = 1, .b = 2, .c = 3, .color = 0xFFFFFFFF},
// 	{.a = 1, .b = 3, .c = 4, .color = 0xFFFFFFFF},
// 	//right
// 	{.a = 4, .b = 3, .c = 5, .color = 0xFFFFFFFF},
// 	{.a = 4, .b = 5, .c = 6, .color = 0xFFFFFFFF},
// 	//back
// 	{.a = 6, .b = 5, .c = 7, .color = 0xFFFFFFFF},
// 	{.a = 6, .b = 7, .c = 8, .color = 0xFFFFFFFF},
// 	//left
// 	{.a = 8, .b = 7, .c = 2, .color = 0xFFFFFFFF},
// 	{.a = 8, .b = 2, .c = 1, .color = 0xFFFFFFFF},
// 	//top
// 	{.a = 2, .b = 7, .c = 5, .color = 0xFFFFFFFF},
// 	{.a = 2, .b = 5, .c = 3, .color = 0xFFFFFFFF},
// 	//bottom
// 	{.a = 6, .b = 8, .c = 1, .color = 0xFFFFFFFF},
// 	{.a = 6, .b = 1, .c = 4, .color = 0xFFFFFFFF},
// };

// white faces with texture coords
face_t cube_faces[N_CUBE_FACES] = {
    // front
    { .a = 1, .b = 2, .c = 3, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
    { .a = 1, .b = 3, .c = 4, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
    // right
    { .a = 4, .b = 3, .c = 5, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
    { .a = 4, .b = 5, .c = 6, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
    // back
    { .a = 6, .b = 5, .c = 7, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
    { .a = 6, .b = 7, .c = 8, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
    // left
    { .a = 8, .b = 7, .c = 2, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
    { .a = 8, .b = 2, .c = 1, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
    // top
    { .a = 2, .b = 7, .c = 5, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
    { .a = 2, .b = 5, .c = 3, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
    // bottom
    { .a = 6, .b = 8, .c = 1, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
    { .a = 6, .b = 1, .c = 4, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF }
};


void load_cube_mesh_data(void){
	int i = 0;
	for(i = 0; i < N_CUBE_VERTICIES; i++){
		array_push(mesh.verticies, cube_verticies[i]);
	}

	for(i = 0; i < N_CUBE_FACES; i++){
		array_push(mesh.faces, cube_faces[i]);
	}
}

void load_obj_file_data(const char* path){

	FILE* file  = fopen(path, "r");
	if(file == NULL){
		printf("file at \"%s\" \n could not be openend", path);
		return;
	}
	char line[512];

	while(fgets(line, sizeof(line), file) != NULL){

		// scan for verts (ex. v 0.437500 0.164062 0.765625)
		if(strncmp(line, "v ", 2) == 0) {
			vec3_t v;
			sscanf(line, "v %f %f %f", &v.x, &v.y, &v.z);
			//printf("vert line: %s", line);
			array_push(mesh.verticies, v);
		}

		// scan for faces (ex. f 47/1/1 1/2/1 3/3/1 45/4/1)
		if(strncmp(line, "f ", 2) == 0){
			
			int vertex_indicies[3];
			int texture_indicies[3];
			int normal_indicies[3];
			//printf("face line: %s", line);
			sscanf(
				line,
				"f %d/%d/%d %d/%d/%d %d/%d/%d",
				&vertex_indicies[0], &texture_indicies[0], &normal_indicies[0],
				&vertex_indicies[1], &texture_indicies[1], &normal_indicies[1],
				&vertex_indicies[2], &texture_indicies[2], &normal_indicies[2]
			);
			face_t face = {
				.a = vertex_indicies[0],
				.b = vertex_indicies[1],
				.c = vertex_indicies[2],
				.color = 0xFFFFFFFF
			};
			
			array_push(mesh.faces, face);
		}
	}
	printf("vert count: %d %f, %f, %f \n",array_length(mesh.verticies), mesh.verticies[0].x, mesh.verticies[0].y, mesh.verticies[0].z);
	printf("face count: %d %d, %d, %d \n",array_length(mesh.faces), mesh.faces[0].a, mesh.faces[0].b, mesh.faces[0].c);

	fclose(file);

}
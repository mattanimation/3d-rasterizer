#include "mesh.h"

mesh_t mesh = {
	.verticies = NULL,
	.faces = NULL,
	.rotation = {0,0,0}
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

face_t cube_faces[N_CUBE_FACES] = {
	//front
	{.a = 1, .b = 2, .c = 3},
	{.a = 1, .b = 3, .c = 4},
	//right
	{.a = 4, .b = 3, .c = 5},
	{.a = 4, .b = 5, .c = 6},
	//back
	{.a = 6, .b = 5, .c = 7},
	{.a = 6, .b = 7, .c = 8},
	//left
	{.a = 8, .b = 7, .c = 2},
	{.a = 8, .b = 2, .c = 1},
	//top
	{.a = 2, .b = 7, .c = 5},
	{.a = 2, .b = 5, .c = 3},
	//bottom
	{.a = 6, .b = 8, .c = 1},
	{.a = 6, .b = 1, .c = 4},
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
			int normals_indicies[3];
			//printf("face line: %s", line);
			sscanf(
				line,
				"f %d/%d/%d %d/%d/%d %d/%d/%d",
				&vertex_indicies[0], &texture_indicies[0], &normals_indicies[0],
				&vertex_indicies[1], &texture_indicies[1], &normals_indicies[1],
				&vertex_indicies[2], &texture_indicies[2], &normals_indicies[2]
			);
			face_t face = {
				.a = vertex_indicies[0],
				.b = vertex_indicies[1],
				.c = vertex_indicies[2]
			};
			
			array_push(mesh.faces, face);
		}
	}
	printf("vert count: %d %f, %f, %f \n",array_length(mesh.verticies), mesh.verticies[0].x, mesh.verticies[0].y, mesh.verticies[0].z);
	printf("face count: %d %d, %d, %d \n",array_length(mesh.faces), mesh.faces[0].a, mesh.faces[0].b, mesh.faces[0].c);

	fclose(file);

}
#include <math.h>
#include "vector.h"


vec3_t vec3_rotate_x(vec3_t v, float angle){
	vec3_t r_vec = {
		.x = v.x,
		.y = v.y * cos(angle) - v.z * sin(angle),
		.z = v.y * sin(angle) + v.z * cos(angle)
	};
	return r_vec;
}

vec3_t vec3_rotate_y(vec3_t v, float angle){
	vec3_t r_vec = {
		.x = v.x * cos(angle) - v.z * sin(angle),
		.y = v.y,
		.z = v.x * sin(angle) + v.z * cos(angle)
	};
	return r_vec;
}

vec3_t vec3_rotate_z(vec3_t v, float angle){
	vec3_t r_vec = {
		.x = v.x * cos(angle) - v.y * sin(angle),
		.y = v.x * sin(angle) + v.y * cos(angle),
		.z = v.z
	};
	return r_vec;
}
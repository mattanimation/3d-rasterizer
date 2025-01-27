#include <math.h>
#include "vector.h"


// float Q_rsqrt( float number ) {
// 	long i;
// 	float x2, y;
// 	const float threehalfs = 1.5F;

// 	x2 = number * 0.5F;
// 	y  = number;
// 	i  = * ( long * ) &y;						// evil floating point bit level hacking
// 	i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
// 	y  = * ( float * ) &i;
// 	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
// //	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

// 	return y;
// }

float Q_rsqrt(float number){
  union {
    float    f;
    uint32_t i;
  } conv = { .f = number };
  conv.i  = 0x5f3759df - (conv.i >> 1);
  conv.f *= 1.5F - (number * 0.5F * conv.f * conv.f);
  return conv.f;
}


///////////////////////////////////////////
// vec2 funcs
///////////////////////////////////////////
float vec2_length(vec2_t v){
	return sqrt(v.x* v.x + v.y * v.y);
}
vec2_t vec2_add(vec2_t a, vec2_t b){
	vec2_t result = {
		.x = a.x + b.x,
		.y = a.y + b.y
	};
	return result;
}
vec2_t vec2_sub(vec2_t a, vec2_t b){
	vec2_t result = {
		.x = a.x - b.x,
		.y = a.y - b.y
	};
	return result;
}
vec2_t vec2_mul(vec2_t v, float factor){
	vec2_t result = {
		.x = v.x * factor,
		.y = v.y * factor
	};
	return result;
}
vec2_t vec2_div(vec2_t v, float factor){
	vec2_t result = {
		.x = v.x / factor,
		.y = v.y / factor
	};
	return result;
}
float vec2_dot(vec2_t a, vec2_t b){
	return (a.x * b.x) + (a.y * b.y);
}
void vec2_normalize(vec2_t* v){
	// another way if could have been done
	//*v = vec2_div(*v,vec2_length(*v));
	
	float l = vec2_length(*v);
	v->x /= l;
	v->y /= l; 
}


///////////////////////////////////////////
// vec3 funcs
///////////////////////////////////////////
float vec3_length(vec3_t v){
	return sqrt(v.x* v.x + v.y * v.y + v.z * v.z);
}
float vec3_length_fast(vec3_t v){
	return Q_rsqrt(v.x* v.x + v.y * v.y + v.z * v.z);
}

vec3_t vec3_add(vec3_t a, vec3_t b){
	vec3_t result = {
		.x = a.x + b.x,
		.y = a.y + b.y,
		.z = a.z + b.z
	};
	return result;
}
vec3_t vec3_sub(vec3_t a, vec3_t b){
	vec3_t result = {
		.x = a.x - b.x,
		.y = a.y - b.y,
		.z = a.z - b.z
	};
	return result;
}

vec3_t vec3_mul(vec3_t v, float factor){
	vec3_t result = {
		.x = v.x * factor,
		.y = v.y * factor,
		.z = v.z * factor
	};
	return result;
}
vec3_t vec3_div(vec3_t v, float factor){
	vec3_t result = {
		.x = v.x / factor,
		.y = v.y / factor,
		.z = v.z / factor
	};
	return result;
}

// get perpendicular vector 
vec3_t vec3_cross(vec3_t a, vec3_t b){
	vec3_t result = {
		.x = a.y * b.z - a.z * b.y,
		.y = a.z * b.x - a.x * b.z,
		.z = a.x * b.y - a.y * b.x
	};
	return result;
}

// a signed -1.0 - 1.0 value
// aligned = 1, perpendicular is 0 and -1 is opposite
float vec3_dot(vec3_t a, vec3_t b){
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

void vec3_normalize(vec3_t* a){
	// another way if could have been done
	//*v = vec3_div(*v, vec3_length(*v));
	float l = vec3_length(*a);
	a->x /= l;
	a->y /= l; 
	a->z /= l;
}

void vec3_normalize_fast(vec3_t* a){
	float l = vec3_length_fast(*a);
	a->x *= l;
	a->y *= l; 
	a->z *= l;
}

// vector conversion functions
vec4_t vec4_from_vec3(vec3_t v){
	vec4_t result = { v.x, v.y, v.z, 1.0 };
	return result;
}

vec3_t vec3_from_vec4(vec4_t v){
	vec3_t result = { v.x, v.y, v.z };
	return result;
}

vec2_t vec2_from_vec4(vec4_t v){
	vec2_t result = { v.x, v.y };
	return result;
}


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
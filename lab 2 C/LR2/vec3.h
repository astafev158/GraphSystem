#pragma once
#ifndef VEC3_H
#define VEC3_H

// Structure for storing point in space
typedef struct{
	float x, y, z;
} vec3;

// Creates vec3
vec3 make_vec(float x, float y, float z);

float dot_product(vec3 a, vec3 b);
vec3 cross_product(vec3 a, vec3 b);
// Multiply vector by matrix 3*3
vec3 vec_multmat3(vec3 v, float*mat);
// Applies rotation to vector
vec3 vec_rotate(vec3 v, vec3 rotation);
// Apply orthographic projection on vector
vec3 project_ortho(vec3 v);
// Apply perspective projection on vector
vec3 project_persp(vec3 v, float rx, float ry, float sx, float sy, float cam_z);

#endif
#include "vec3.h"
#include <math.h>

// Creates vec3 struct
vec3 make_vec(float x, float y, float z){
	vec3 ret;
	ret.x = x;
	ret.y = y;
	ret.z = z;
	return ret;
}

float dot_product(vec3 a, vec3 b){
	return a.x*b.x + a.y*b.y + a.z*b.z;
}
vec3 cross_product(vec3 a, vec3 b){
	return make_vec(
		a.y*b.z - a.z*b.y,
		a.z*b.x - a.x*b.z,
		a.x*b.y - a.y*b.x
	);
}
// Multiply vector by matrix 3*3
vec3 vec_multmat3(vec3 v, float*mat){
	return make_vec(
		v.x*mat[0] + v.y*mat[1] + v.z*mat[2],
		v.x*mat[3] + v.y*mat[4] + v.z*mat[5],
		v.x*mat[6] + v.y*mat[7] + v.z*mat[8]
	);
}
// Applies rotation to vector
vec3 vec_rotate(vec3 v, vec3 rotation){
	const float deg2rad = 3.1415f / 180.0f;
	vec3 rad = make_vec(rotation.x*deg2rad, rotation.y*deg2rad, rotation.z*deg2rad);
	float matrix[9] = {
		cos(rad.x)*cos(rad.y) - sin(rad.x)*sin(rad.z)*sin(rad.y), -sin(rad.x)*cos(rad.z), -cos(rad.x)*sin(rad.y) - sin(rad.x)*sin(rad.z)*cos(rad.y),
		cos(rad.x)*sin(rad.z)*sin(rad.y) + sin(rad.x)*cos(rad.y), cos(rad.x)*cos(rad.z), cos(rad.x)*sin(rad.z)*cos(rad.y) - sin(rad.x)*sin(rad.y),
		cos(rad.z)*sin(rad.y), -sin(rad.z), cos(rad.z)*cos(rad.y)
	};
	return vec_multmat3(v, matrix);
}
// Apply orthographic projection on vector
vec3 project_ortho(vec3 v){
	return make_vec(v.x, -v.y, 0);
}
// Apply perspective projection on vector
vec3 project_persp(vec3 v, float rx, float ry, float sx, float sy, float cam_z){
	vec3 r = make_vec(rx, ry, 1);
	return make_vec(
		(v.x*sx*1) / ((v.z - cam_z) * r.x) * r.z, //
		(v.y*sy*1) / ((v.z - cam_z) * r.y) * r.z,
		0
	);
}
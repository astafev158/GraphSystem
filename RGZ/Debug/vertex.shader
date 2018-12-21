#version 140
#extension GL_ARB_explicit_attrib_location : enable
layout (location = 0) in vec3 vp;
uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
void main() {
	gl_Position = proj * view * model * vec4(vp, 1.0);
}
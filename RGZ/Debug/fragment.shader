#version 140
out vec4 fc;
uniform vec3 col;
void main() {
	fc = vec4(col, 1.0);
}

#include <GL/glew.h> // The OpenGL Extension Wrangler Library
#include <GLFW/glfw3.h> // OpenGL Framework
#include <CGLM/cglm.h> // C OpenGL Math
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include "obj.h"
#include "settings_window.h"
#define VEC3(x,y,z) ((vec3){x,y,z}) // Easy macros for vec3 initialization
#pragma warning (disable:4996);
/// Struct for storing figure buffers
typedef struct {
	int count;
	GLuint *VAO, *VBO;
} figure; 

int fig_count; // Total amount of figures
figure*figs; // Figures array
figure*fig_current; // Currently selected figure
vec3 translate; // Translation vector
vec3 rotate; // Rotation vector
vec3 scale; // Scale vector

/// Returns file contents
const char*read_file(const char*path) {
	FILE*file;
	long size;
	char*buffer;

	if (fopen_s(&file, path, "r"))
		return NULL;
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);
	buffer = calloc(size + 1, 1);
	fread(buffer, 1, size, file);
	fclose(file);
	return buffer;
}
/// Compiles shader of from file
GLboolean create_shader(const char*shader_path, GLenum type, GLuint*shader) {
	GLint success;
	GLint log_size;
	GLchar*log;
	const char*source = read_file(shader_path); // Trying to read shader source
	if (!source) {
		puts("Failed to read shader file");
		return GL_FALSE;
	}
	*shader = glCreateShader(type); // Allocating shader
	glShaderSource(*shader, 1, &source, NULL); // Assigning shader source
	glCompileShader(*shader); // Compiling shader
	glGetShaderiv(*shader, GL_COMPILE_STATUS, &success); // Checking if compilation was successful
	if (!success) { // If not
		puts("Failed to compile shader");
		glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &log_size); // Get error length
		log = calloc(log_size + 1, 1); // Allocate memory
		glGetShaderInfoLog(*shader, log_size, NULL, log); // Get error string
		puts(log); // Print to console
		free(log);
		return GL_FALSE;
	}
	return GL_TRUE;
}
/// Compiles shaders to shader program
GLboolean create_program(const char*vshader_path, const char*fshader_path, GLuint*program) {
	GLuint vshader;
	GLuint fshader;
	GLint success;
	GLint log_size;
	GLchar*log;

	// Trying to create vertex shader
	if (!create_shader(vshader_path, GL_VERTEX_SHADER, &vshader)) {
		puts("Vertex shader creation failed");
		return GL_FALSE;
	}
	// Trying to create fragment shader
	if (!create_shader(fshader_path, GL_FRAGMENT_SHADER, &fshader)) {
		puts("Fragment shader creation failed");
		return GL_FALSE;
	}
	*program = glCreateProgram(); // Allocating shader program
	glAttachShader(*program, vshader); // Attaching shaders
	glAttachShader(*program, fshader);
	glLinkProgram(*program); // Linking program
	glDeleteShader(vshader); // Freeing shaders
	glDeleteShader(fshader);
	glGetProgramiv(*program, GL_LINK_STATUS, &success); // Checking success
	if (!success) { // If failed
		puts("Failed to link shader program");
		glGetProgramiv(*program, GL_INFO_LOG_LENGTH, &log_size); // Get error length
		log = calloc(log_size + 1, 1); // Allocate string
		glGetProgramInfoLog(program, log_size, NULL, log); // Get error string
		puts(log); // Print error
		free(log);
		return GL_FALSE;
	}
	return GL_TRUE;
}
/// Prints OpenGL version string to console
void print_gl_info() {
	const GLubyte*rend = glGetString(GL_RENDERER); // OpenGL renderer information
	const GLubyte*vers = glGetString(GL_VERSION); // OpenGL version information
	printf("Renderer: %s\n", rend);
	printf("OpenGL version: %s\n", vers);
}
/// Converts .obj model to OpenGL buffers
void obj_to_gl(obj o, int*count, GLuint**vao, GLuint**vbo) {
	*count = o.fc;
	GLuint*VAO = malloc(sizeof(GLuint)**count); // Allocate buffer arrays
	GLuint*VBO = malloc(sizeof(GLuint)**count);
	glGenVertexArrays(*count, VAO); // Allocate buffers
	glGenBuffers(*count, VBO);

	for (int i = 0; i < o.fc; ++i) { // One VAO and one VBO for each model face
		int vb_size = sizeof(float)*o.f[i].count * 3; // Precalculate VBO size
		float*verts = malloc(vb_size); // Allocate temporary buffer
		for (int j = 0; j < o.f[i].count; ++j) {
			verts[j * 3 + 0] = o.v[o.f[i].v[j]].x; // Flatten .obj model face to buffer
			verts[j * 3 + 1] = o.v[o.f[i].v[j]].y;
			verts[j * 3 + 2] = o.v[o.f[i].v[j]].z;
		}
		glBindVertexArray(VAO[i]); // Bind VAO
		glBindBuffer(GL_ARRAY_BUFFER, VBO[i]); // Bind VBO to it
		glBufferData(GL_ARRAY_BUFFER, vb_size, verts, GL_STATIC_DRAW); // Fill VBO
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0); // Set shader parameter
		glEnableVertexAttribArray(0); // Enable parameter
	}
	glBindVertexArray(0); // Unbind VAO
	*vao = VAO;
	*vbo = VBO;
}
/// Create figure from .obj file
figure create_figure(const char*obj_path) {
	figure fig;
	obj o,t;
	read_obj(obj_path, &t); // Reading .obj mode
	o = obj_triangulate(t); // Triangulating it
	obj_to_gl(o, &fig.count, &fig.VAO, &fig.VBO); // Converting to buffers
	return fig;
}
/// Initializes predefined figures and it's buttons
void init_figures(HWND hWnd, figure**figures, HWND**buttons, int*count) {
	*count = 5;
	*figures = malloc(sizeof(figure*) * 5); // Allocate arrays
	*buttons = malloc(sizeof(HWND*) * 5);
	(*figures)[0] = create_figure("tetra.obj"); // Read figures
	(*figures)[1] = create_figure("cube.obj");
	(*figures)[2] = create_figure("octa.obj");
	(*figures)[3] = create_figure("dodeca.obj");
	char str[255];
	char strr[5]=".obj";
	

	puts("name model dopolnitelno? primer:cow,tea ");
	gets(str);
	strcat(str, strr);
	(*figures)[4] = create_figure(str);
	(*buttons)[0] = add_button(hWnd, 0, 00, 100, 22, _T("Тетраэдр")); // Add buttons
	(*buttons)[1] = add_button(hWnd, 0, 22, 100, 22, _T("Гексаэдр"));
	(*buttons)[2] = add_button(hWnd, 0, 44, 100, 22, _T("Октаэдр"));
	(*buttons)[3] = add_button(hWnd, 0, 66, 100, 22, _T("Додекаэдр"));
	(*buttons)[4] = add_button(hWnd, 0, 88, 100, 22, _T("доп фигура"));
	fig_current = &((*figures)[0]); // Select first figure
	figs = *figures; // Store figures to global
}
/// Updates transformation hint text
void update_label(HWND text, float value) {
	char*buffer = malloc(16);
	sprintf_s(buffer, 16, "%.1f", value);
	SetWindowTextA(text, buffer);
	free(buffer);
}
/// Called when button is clicked
void CALLBACK on_settings_click(HWND hWnd, HWND hControl, LPARAM lParam) {
	LPVOID*data = lParam; // Extract data
	int count = data[0]; // Extract figure count
	figure*figures = data[1]; // Extract figures
	HWND*fig_buttons = data[2]; // Extract figure buttons
	HWND*buttons = data[3]; // Extract buttons
	HWND*texts = data[4]; // Extract labels
	for (int i = 0; i < count; ++i) // Check if figure button is clicked
		if (hControl == fig_buttons[i]) {
			fig_current = &figures[i]; // If so, select corresponding figure
			return;
		}
	// Check which transformation button is clicked
	if (hControl == buttons[0]) translate[0] -= .1f; // Pos X-
	if (hControl == buttons[1]) translate[0] += .1f; // Pos X+
	if (hControl == buttons[2]) translate[1] -= .1f; // Pos Y-
	if (hControl == buttons[3]) translate[1] += .1f; // Pos Y+
	if (hControl == buttons[4]) translate[2] -= .1f; // Pos Z-
	if (hControl == buttons[5]) translate[2] += .1f; // Pos Z+

	if (hControl == buttons[6]) rotate[0] -= .1f; // Rot X-
	if (hControl == buttons[7]) rotate[0] += .1f; // Rot X+
	if (hControl == buttons[8]) rotate[1] -= .1f; // Rot Y-
	if (hControl == buttons[9]) rotate[1] += .1f; // Rot Y+
	if (hControl == buttons[10]) rotate[2] -= .1f; // Rot Z-
	if (hControl == buttons[11]) rotate[2] += .1f; // Rot Z+

	if (hControl == buttons[12]) scale[0] -= .1f; // Scale X-
	if (hControl == buttons[13]) scale[0] += .1f; // Scale X+
	if (hControl == buttons[14]) scale[1] -= .1f; // Scale Y-
	if (hControl == buttons[15]) scale[1] += .1f; // Scale Y+
	if (hControl == buttons[16]) scale[2] -= .1f; // Scale Z-
	if (hControl == buttons[17]) scale[2] += .1f; // Scale Z+

	// Check reset buttons
	if (hControl == buttons[18]) { // Reset Pos
		translate[0] = 0;
		translate[1] = 0;
		translate[2] = 0;
	}
	if (hControl == buttons[19]) { // Reset Rot
		rotate[0] = 0;
		rotate[1] = 0;
		rotate[2] = 0;
	}
	if (hControl == buttons[20]) { // Reset Scale
		scale[0] = 1;
		scale[1] = 1;
		scale[2] = 1;
	}

	// Update labels
	update_label(texts[0], translate[0]); // Pos
	update_label(texts[1], translate[1]);
	update_label(texts[2], translate[2]);
	update_label(texts[3], rotate[0]);    // Rot
	update_label(texts[4], rotate[1]);
	update_label(texts[5], rotate[2]);
	update_label(texts[6], scale[0]);     // Scale
	update_label(texts[7], scale[1]);
	update_label(texts[8], scale[2]);
}
/// Initialize settings window
void init_settings() {
	LPVOID*data = malloc(sizeof(LPVOID) * 5); // Allocate data to pass to callback
	HWND hWnd = create_window(on_settings_click, data, 458, 148); // Create window
	init_figures(hWnd, &data[1], &data[2], &data[0]); // Initialize figures
	HWND*btns = malloc(sizeof(HWND) * 21); // Allocate transformation buttons array
	HWND*texts = malloc(sizeof(HWND) * 9); // Allocate transformation value hints array

	add_text(hWnd, 112, 0, 100, 22, _T("Pos")); // Hints
	add_text(hWnd, 222, 0, 100, 22, _T("Rot"));
	add_text(hWnd, 332, 0, 100, 22, _T("Scale"));

	btns[0] = add_button(hWnd, 162, 22, 25, 22, _T("X-")); // Pos
	btns[1] = add_button(hWnd, 187, 22, 25, 22, _T("X+"));
	btns[2] = add_button(hWnd, 162, 44, 25, 22, _T("Y-"));
	btns[3] = add_button(hWnd, 187, 44, 25, 22, _T("Y+"));
	btns[4] = add_button(hWnd, 162, 66, 25, 22, _T("Z-"));
	btns[5] = add_button(hWnd, 187, 66, 25, 22, _T("Z+"));
	texts[0] = add_text(hWnd, 112, 22, 50, 22, _T(""));
	texts[1] = add_text(hWnd, 112, 44, 50, 22, _T(""));
	texts[2] = add_text(hWnd, 112, 66, 50, 22, _T(""));

	btns[6] = add_button(hWnd, 272, 22, 25, 22, _T("X-")); // Rot
	btns[7] = add_button(hWnd, 297, 22, 25, 22, _T("X+"));
	btns[8] = add_button(hWnd, 272, 44, 25, 22, _T("Y-"));
	btns[9] = add_button(hWnd, 297, 44, 25, 22, _T("Y+"));
	btns[10] = add_button(hWnd, 272, 66, 25, 22, _T("Z-"));
	btns[11] = add_button(hWnd, 297, 66, 25, 22, _T("Z+"));
	texts[3] = add_text(hWnd, 222, 22, 50, 22, _T(""));
	texts[4] = add_text(hWnd, 222, 44, 50, 22, _T(""));
	texts[5] = add_text(hWnd, 222, 66, 50, 22, _T(""));

	btns[12] = add_button(hWnd, 382, 22, 25, 22, _T("X-")); // Scale
	btns[13] = add_button(hWnd, 407, 22, 25, 22, _T("X+"));
	btns[14] = add_button(hWnd, 382, 44, 25, 22, _T("Y-"));
	btns[15] = add_button(hWnd, 407, 44, 25, 22, _T("Y+"));
	btns[16] = add_button(hWnd, 382, 66, 25, 22, _T("Z-"));
	btns[17] = add_button(hWnd, 407, 66, 25, 22, _T("Z+"));
	texts[6] = add_text(hWnd, 332, 22, 50, 22, _T(""));
	texts[7] = add_text(hWnd, 332, 44, 50, 22, _T(""));
	texts[8] = add_text(hWnd, 332, 66, 50, 22, _T(""));

	btns[18] = add_button(hWnd, 162, 88, 50, 22, _T("Reset")); // Pos
	btns[19] = add_button(hWnd, 272, 88, 50, 22, _T("Reset")); // Rot
	btns[20] = add_button(hWnd, 382, 88, 50, 22, _T("Reset")); // Scale

	data[3] = btns; // Store it into data
	data[4] = texts;

	on_settings_click(hWnd, NULL, data); // Kinda hacky way to update labels
}

int main() {
	GLFWwindow*window; // Main rendering window
	GLuint program; // Shader program
	GLint col_loc, model_loc, proj_loc, view_loc; // Shader uniforms
	mat4 model, proj, view; // Matrices
	int w, h; // Viewbuffer width and height

	if (!glfwInit()) { // Try to initialize GLFW
		puts("Failed to initialize GLFW");
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2); // Require OpenGL at least 2.0
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // Forbid window resizing

	window = glfwCreateWindow(400, 400, "RGZ", NULL, NULL); // Create render window
	if (!window) {
		glfwTerminate();
		puts("Failed to initialize window");
		return -1;
	}
	glfwMakeContextCurrent(window); // Assing render context to it
	glEnable(GL_DEPTH_TEST); // Enable depth
	glDepthFunc(GL_LEQUAL);
	glfwSwapInterval(1); // Slow down rendering a bit
	if (glewInit()) { // Try to initialize GLEW
		puts("Failed to initialize GLEW");
		return -1;
	}
	print_gl_info(); // Print OpenGL information
	if (!create_program("vertex.shader", "fragment.shader", &program)) { // Load shaders
		glfwDestroyWindow(window);
		glfwTerminate();
		puts("Failed to create shader program");
		return -1;
	}

	col_loc = glGetUniformLocation(program, "col"); // Get uniform locations
	model_loc = glGetUniformLocation(program, "model");
	proj_loc = glGetUniformLocation(program, "proj");
	view_loc = glGetUniformLocation(program, "view");

	translate[0] = 0; // Set default transformations
	translate[1] = 0;
	translate[2] = 0;
	rotate[0] = 0;
	rotate[1] = 0;
	rotate[2] = 0;
	scale[0] = 1;
	scale[1] = 1;
	scale[2] = 1;
	init_settings(); // Initialize settings window
	glfwGetFramebufferSize(window, &w, &h); // Get framebuffer size
	glm_perspective(glm_rad(45.f), (float)w / h, .1f, 100.f, proj); // Get projection matrix
	glm_lookat(VEC3(0, 2, 3), GLM_VEC3_ZERO, VEC3(0, 1, 0), view); // Get view matrix
	glUseProgram(program); // Activate shader program
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, &proj[0][0]); // Store projection and view matrices
	glUniformMatrix4fv(view_loc, 1, GL_FALSE, &view[0][0]); // to uniforms
	glLineWidth(2.f); // Increase line width
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set default render context color

	while (!glfwWindowShouldClose(window)) { // Rendering loop
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
		
		glm_mat4_copy(GLM_MAT4_IDENTITY, model); // Calculate model matrix
		glm_translate(model, translate);
		glm_rotate(model, glm_rad(45 * rotate[0]), GLM_XUP);
		glm_rotate(model, glm_rad(45 * rotate[1]), GLM_YUP);
		glm_rotate(model, glm_rad(45 * rotate[2]), GLM_ZUP);
		glm_scale(model, scale);
		glUniformMatrix4fv(model_loc, 1, GL_FALSE, &model[0][0]); // Store model matrix

		if (fig_current) { // If any figure is selected
			for (int i = 0; i < fig_current->count; ++i) { // Draw each of its faces
				glBindVertexArray(fig_current->VAO[i]); // Activate face VAO
				glUniform3f(col_loc, .0f, .0f, .0f); // Set face color (same as background so its invisible
				glDrawArrays(GL_TRIANGLES, 0, 3); // Draw solid face
				glUniform3f(col_loc, 1.f, 1.f, 1.f); // Set edge color
				glDrawArrays(GL_LINE_LOOP, 0, 3); // Draw face edges
			}
		}

		glfwPollEvents(); // Pump GLFW events
		glfwSwapBuffers(window); // Swap back buffer
	}

	for (int i = 0; i < fig_count; ++i) { // Release buffers
		glDeleteVertexArrays(figs[i].count, figs[i].VAO);
		glDeleteBuffers(figs[i].count, figs[i].VBO);
		free(figs[i].VAO);
		free(figs[i].VBO);
	}
	glDeleteProgram(program); // Release shader program
	glfwDestroyWindow(window); // Release window

	glfwTerminate(); // Terminate GLFW
	return(0);
}
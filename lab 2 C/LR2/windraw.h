#pragma once
#ifndef WINDRAW_H
#define WINDRAW_H
#define WINDRAW_SIZE 400
#define PROJECTION_ORTHO 1
#define PROJECTION_PERSPECTIVE 2
#include <Windows.h>
#include "vec3.h"

// Callback for handling WM_PAINT event
typedef void(CALLBACK*paint_callback)(HWND, HDC);
// Callback for handling WM_LBUTTONDOWN event
typedef void(CALLBACK*click_callback)(HWND, POINT);
// Callback for handling WM_DESTROY event
typedef void(CALLBACK*destroy_callback)(HWND);
// Callback for handling WM_COMMAND event
typedef void(CALLBACK*command_callback)(HWND, WORD, WORD, HWND);
// Structure for storing color in RGB [0,1] space
typedef struct {
	float r, g, b;
} rgb;
// Structure for storing triangle in space
typedef struct {
	vec3 vertices[3];
} face;
// Structure for storing figure with set amount of faces
typedef struct {
	face*faces;
	int amount;
} figure;
// Structure for storing callbacks to use in graphics window events
typedef struct {
	paint_callback cb_paint;
	click_callback cb_click;
	destroy_callback cb_destroy;
	command_callback cb_command;
} callbacks;

// Creates rgb
rgb make_rgb(float r, float g, float b);
// Creates face
face make_face(vec3 a, vec3 b, vec3 c);
// Creates figure (faces are copied)
figure make_fig(int amount, face*faces);
// Creates figure from triangles list (count - amount of faces)
figure fig_trilist(int count, ...);
// Creates figure from triangles strip (count - amount of faces)
figure fig_tristrip(int count, ...);
// Creates figure from indexed triangles list (count - amount of faces)
figure fig_trilist_index(int count, vec3*vertices, int*indices);
// Creates figure from indexed triangles strip (count - amount of faces)
figure fig_tristrip_index(int count, vec3*vertices, int*indices);

// Initializes graphics window with specified callbacks
HWND init_window(callbacks cb);
// Runs window event pump
WPARAM window_loop(HWND hWnd);

// Creates HPEN from specified color and style (PS_* flags)
HPEN create_pen(rgb color, int style);
// Creates HBRUSH from specified color
HBRUSH create_brush(rgb color);
// Draws line between points using specified pen
void draw_line(HDC hdc, vec3 from, vec3 to, HPEN pen);
// Draws face using specified pen and brush
void draw_face(HDC hdc, face face, vec3 rotation, DWORD projection, HPEN pen, HBRUSH brush);
// Draws face wireframe using specified pen
void draw_face_wire(HDC hdc, face face, vec3 rotation, DWORD projection, HPEN pen);
// Draws figure using specified pen and brush
void draw_figure(HDC hdc, figure fig, vec3 rotation, DWORD projection, HPEN pen, HBRUSH brush);
// Draws figure wireframe using specified pen
void draw_figure_wire(HDC hdc, figure fig, vec3 rotation, DWORD projection, HPEN pen);
// Fills canvas with default window color
void draw_clear(HDC hdc);
// Draws text on specified point
void draw_text(HDC hdc, float x, float y, LPCTSTR text);

#endif
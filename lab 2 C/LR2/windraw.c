#include "windraw.h"
#include <windowsx.h>
#include <tchar.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

typedef struct {
	HWND hWnd;
	callbacks cb;
	HBITMAP buffer;
} window_data;

// Returns TRUE if face should not be rendered
BOOL should_cull(face f, DWORD projection) {
	vec3 a = f.vertices[0];
	vec3 b = f.vertices[1];
	vec3 c = f.vertices[2];
	vec3 norm = cross_product(
		make_vec(b.x - a.x, b.y - a.y, b.z - a.z),
		make_vec(c.x - a.x, c.y - a.y, c.z - a.z)
	);
	if(projection == PROJECTION_ORTHO)
		return norm.z < 0;
	return dot_product(
		make_vec(a.x, a.y, a.z - 2),
		norm
	) >= 0;
}
// Translates world vec3 [-1,1] to window POINT [0,WINDRAW_SIZE]
POINT translate_vec(vec3 v) {
	POINT pt;
	pt.x = (v.x*WINDRAW_SIZE + WINDRAW_SIZE) / 2;
	pt.y = (v.y*WINDRAW_SIZE + WINDRAW_SIZE) / 2;
	return pt;
}
// Applies rotation to face
face face_rotate(face f, vec3 rotation) {
	return make_face(
		vec_rotate(f.vertices[0], rotation),
		vec_rotate(f.vertices[1], rotation),
		vec_rotate(f.vertices[2], rotation)
	);
}
// Calculates face's normal vector
vec3 face_normal(face f, vec3 angle) {
	vec3 a = vec_rotate(f.vertices[0], angle);
	vec3 b = vec_rotate(f.vertices[1], angle);
	vec3 c = vec_rotate(f.vertices[2], angle);

	vec3 norm = cross_product(
		make_vec(b.x - a.x, b.y - a.y, b.z - a.z),
		make_vec(c.x - a.x, c.y - a.y, c.z - a.z)
	);

	return norm;
}
// Applies projection on vector
vec3 vec_project(vec3 v, DWORD persp) {
	if(persp == PROJECTION_PERSPECTIVE)
		return project_persp(v, WINDRAW_SIZE, WINDRAW_SIZE, WINDRAW_SIZE, WINDRAW_SIZE, 2);
	return project_ortho(v);
}
// Graphics window procedure
LRESULT CALLBACK wnd_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	window_data*wd = GetWindowLongPtr(hWnd, GWLP_USERDATA); // Extracting window_data from window userdata
	if (uMsg == WM_CREATE) { // Window has been created
		LPCREATESTRUCT cs = lParam;
		RECT client;
		SIZE window;
		wd = cs->lpCreateParams;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, cs->lpCreateParams); // Store window_data to window userdata
		GetClientRect(hWnd, &client);
		window.cx = WINDRAW_SIZE + WINDRAW_SIZE - client.right + client.left;
		window.cy = WINDRAW_SIZE + WINDRAW_SIZE - client.bottom + client.top; // Resize window to match client size with WINDRAW_SIZE
		SetWindowPos(hWnd, NULL, NULL, NULL, window.cx, window.cy, SWP_NOZORDER | SWP_NOMOVE);
	}
	// Window repaint
	if (uMsg == WM_PAINT && wd->cb.cb_paint) {
		PAINTSTRUCT ps;
		BeginPaint(hWnd, &ps);
		if (wd->buffer == NULL) // Create buffer if this is the first time we draw
			wd->buffer = CreateCompatibleBitmap(ps.hdc, WINDRAW_SIZE * 2, WINDRAW_SIZE * 2);
		HDC buf_hdc = CreateCompatibleDC(ps.hdc);
		SelectBitmap(buf_hdc, wd->buffer);
		wd->cb.cb_paint(hWnd, buf_hdc); // Call paint callback on buffer HDC
		BitBlt(ps.hdc, 0, 0, WINDRAW_SIZE, WINDRAW_SIZE, buf_hdc, 0, 0, SRCCOPY);
		DeleteDC(buf_hdc);
		EndPaint(hWnd, &ps);
		return NULL;
	}
	if (uMsg == WM_LBUTTONDOWN && wd->cb.cb_click) {
		POINT pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		wd->cb.cb_click(hWnd, pt); // Call callback with mouse coordinates
	}
	if (uMsg == WM_DESTROY) { // Window is being destroyed. Free resources
		DeleteBitmap(wd->buffer);
		free(wd);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, NULL);
		PostQuitMessage(0);
		return 0;
	}
	if (uMsg == WM_COMMAND && wd->cb.cb_command) { // Received notification from child control
		WORD wControl = LOWORD(wParam);
		WORD wCode = HIWORD(wParam);
		HWND hControl = lParam;
		wd->cb.cb_command(hWnd, wControl, wCode, hControl);
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
// Initialize graphics window with passed callbacks and show it
HWND init_window(callbacks cb) {
	// Get current HINSTANCE from console window
	HINSTANCE hInstance = GetWindowLongPtr(GetConsoleWindow(), GWLP_HINSTANCE);
	LPCTSTR class_name = _T("class_name"); // WNDCLASS.lpszClassName
	WNDCLASS class;
	window_data*wd = malloc(sizeof(window_data)); // Store callbacks struct in heap
	wd->cb = cb;
	wd->buffer = NULL;
	class.style = CS_HREDRAW | CS_VREDRAW;
	class.lpfnWndProc = wnd_proc;
	class.cbClsExtra = 0;
	class.cbWndExtra = 0;
	class.hInstance = hInstance;
	class.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	class.hCursor = LoadCursor(NULL, IDC_ARROW);
	class.hbrBackground = COLOR_WINDOW + 1;
	class.lpszMenuName = NULL;
	class.lpszClassName = class_name;
	RegisterClass(&class);
	HWND hWnd = CreateWindow(
		class_name,
		_T(""),
		WS_SYSMENU | WS_CAPTION | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		WINDRAW_SIZE, WINDRAW_SIZE,
		NULL, NULL,
		hInstance, wd
	);
	wd->hWnd = hWnd;
	return hWnd;
}
// Graphics window event pump
WPARAM window_loop(HWND hWnd) {
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
// Creates rgb struct
rgb make_rgb(float r, float g, float b) {
	rgb ret;
	ret.r = r;
	ret.g = g;
	ret.b = b;
	return ret;
}
// Creates face struct
face make_face(vec3 a, vec3 b, vec3 c) {
	face ret;
	ret.vertices[0] = a;
	ret.vertices[1] = b;
	ret.vertices[2] = c;
	return ret;
}
// Creates figure struct (faces are copied)
figure make_fig(int amount, face*faces) {
	figure ret;
	ret.amount = amount;
	ret.faces = malloc(sizeof(face)*amount);
	memcpy(ret.faces, faces, sizeof(face)*amount);
	return ret;
}
// Creates figure struct from triangles list
figure fig_trilist(int count, ...) {
	va_list ap;
	int vc = count * 3;
	vec3*verts = malloc(sizeof(vec3)*vc);
	va_start(ap, count);
	for (int i = 0; i < vc; ++i)
		verts[i] = va_arg(ap, vec3);
	va_end(ap);
	face*faces = malloc(sizeof(face)*count);
	for (int i = 0; i < count; ++i) {
		faces[i].vertices[0] = verts[i * 3 + 0];
		faces[i].vertices[1] = verts[i * 3 + 1];
		faces[i].vertices[2] = verts[i * 3 + 2];
	}
	figure fig;
	fig.amount = count;
	fig.faces = faces;
	free(verts);
	return fig;
}
// Creates figure struct from triangles strip
figure fig_tristrip(int count, ...) {
	va_list ap;
	int vc = count + 2;
	vec3*verts = malloc(sizeof(vec3)*vc);
	va_start(ap, count);
	for (int i = 0; i < vc; ++i)
		verts[i] = va_arg(ap, vec3);
	va_end(ap);
	face*faces = malloc(sizeof(face)*count);
	for (int f = 0; f < count; ++f) {
		faces[f].vertices[0] = verts[f + 0];
		faces[f].vertices[1] = verts[f + 1];
		faces[f].vertices[2] = verts[f + 2];
	}
	figure fig;
	fig.amount = count;
	fig.faces = faces;
	free(verts);
	return fig;
}
// Creates figure from indexed triangles list (count - amount of faces)
figure fig_trilist_index(int count, vec3*vertices, int*indices){
	figure fig;
	fig.amount = count;
	fig.faces = malloc(sizeof(face)*count);
	for(int i = 0; i < count; ++i)
		fig.faces[i] = make_face(
			vertices[indices[i * 3 + 0]],
			vertices[indices[i * 3 + 1]],
			vertices[indices[i * 3 + 2]]
		);
	return fig;
}
// Creates figure from indexed triangles strip (count - amount of faces)
figure fig_tristrip_index(int count, vec3*vertices, int*indices){
	figure fig;
	fig.amount = count;
	fig.faces = malloc(sizeof(face)*count);
	for(int i = 0; i < count; ++i)
		fig.faces[i] = make_face(
			vertices[indices[i + 0]],
			vertices[indices[i + 1]],
			vertices[indices[i + 2]]
		);
	return fig;
}

// Creates HPEN from rgb struct (style is PS_* flags)
HPEN create_pen(rgb color, int style) {
	return CreatePen(style, 1, RGB(color.r * 256, color.g * 256, color.b * 256));
}
// Creates solid color HBRUSH from rgb struct
HBRUSH create_brush(rgb color) {
	return CreateSolidBrush(RGB(color.r * 255, color.g * 255, color.b * 255));
}
// Draws line between point using specified pen
void draw_line(HDC hdc, vec3 from, vec3 to, HPEN pen) {
	if (pen) SelectPen(hdc, pen);
	POINT pt1 = translate_vec(from);
	POINT pt2 = translate_vec(to);
	MoveToEx(hdc, pt1.x, pt1.y, NULL);
	LineTo(hdc, pt2.x, pt2.y);
}
// Draws face using specified pen and brush
void draw_face(HDC hdc, face face, vec3 rotation, DWORD projection, HPEN pen, HBRUSH brush) {
	POINT points[3] = {
		translate_vec(vec_project(vec_rotate(face.vertices[0], rotation), projection)),
		translate_vec(vec_project(vec_rotate(face.vertices[1], rotation), projection)),
		translate_vec(vec_project(vec_rotate(face.vertices[2], rotation), projection))
	};

	if (pen) SelectPen(hdc, pen);
	if (brush) SelectBrush(hdc, brush);
	Polygon(hdc, points, 3);
}
// Draws face wireframe using specified pen
void draw_face_wire(HDC hdc, face face, vec3 rotation, DWORD projection, HPEN pen) {
	POINT points[4] = {
		translate_vec(vec_project(vec_rotate(face.vertices[0], rotation), projection)),
		translate_vec(vec_project(vec_rotate(face.vertices[1], rotation), projection)),
		translate_vec(vec_project(vec_rotate(face.vertices[2], rotation), projection)),
		translate_vec(vec_project(vec_rotate(face.vertices[0], rotation), projection))
	};

	if (pen) SelectPen(hdc, pen);
	Polyline(hdc, points, 4);
}
// Draws figure using specified pen and brush
void draw_figure(HDC hdc, figure fig, vec3 rotation, DWORD projection, HPEN pen, HBRUSH brush) {
	if (pen) SelectPen(hdc, pen);
	if (brush) SelectBrush(hdc, brush);

	for (int i = 0; i < fig.amount; ++i)
		if(!should_cull(face_rotate(fig.faces[i], rotation), projection))
			draw_face(hdc, fig.faces[i], rotation, projection, NULL, NULL);
}
// Draws figure wireframe using specified pen
void draw_figure_wire(HDC hdc, figure fig, vec3 rotation, DWORD projection, HPEN pen) {
	if (pen) SelectPen(hdc, pen);
	for (int i = 0; i < fig.amount; ++i)
		draw_face_wire(hdc, fig.faces[i], rotation, projection, NULL, NULL);
}
// Fills whole canvas with default window color (COLOR_WINDOW)
void draw_clear(HDC hdc) {
	RECT rect;
	rect.right = WINDRAW_SIZE;
	rect.bottom = WINDRAW_SIZE;
	rect.left = -WINDRAW_SIZE;
	rect.top = -WINDRAW_SIZE;

	FillRect(hdc, &rect, COLOR_WINDOW + 1);
}
// Draws text at specified point
void draw_text(HDC hdc, float x, float y, LPCTSTR text) {
	POINT pt = translate_vec(make_vec(x, y, 0));
	int len = _tcslen(text);
	TextOut(hdc, pt.x, pt.y, text, len);
}
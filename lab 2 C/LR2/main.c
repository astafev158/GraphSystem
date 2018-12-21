#include "windraw.h"
#include "wintimer.h"
#include "figures.h"
#include <tchar.h>
#include <math.h>
#include <stdio.h>
#define _tputs(str) _tprintf(_T("%s\n"), str);

// Makes sure angle is in [0,360] interval
float loop_angle(float a){
	while(a < 0)
		a = 360.0 + a;
	return fmod(a, 360);
}

// Pen for faces' borders
HPEN pen;
// Brush to fill faces
HBRUSH brush;
// Array of predefined figures
figure*figures;
// Array of names of predefined figures
LPCTSTR*fig_names;
// Amount of predefined figures
int fig_count;
// Index of currently selected predefined figure
int fig_sel = 0;
// Array of buttons
HWND*btn_figures;
HWND buttons[9];
// Animation timer
LPWINTIMER timer;
// Rotation angle
vec3 rotation;
// Projection mode
DWORD projection_mode = PROJECTION_PERSPECTIVE;
// Temporary buffer for rotation angle string
LPTSTR rotation_string;
// If true, object will be rotating on its own
BOOL animation = TRUE;
// If true, objects' faces will be visible
BOOL fill = TRUE;

// Callback for WM_PAINT event
void CALLBACK on_paint(HWND hWnd, HDC hdc){
	draw_clear(hdc); // Clear canvas
	if(fill) // Draw selected figure
		draw_figure(hdc, figures[fig_sel], rotation, projection_mode, pen, brush);
	else
		draw_figure_wire(hdc, figures[fig_sel], rotation, projection_mode, pen);
	SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
	SetTextAlign(hdc, TA_BOTTOM | TA_CENTER);
	draw_text(hdc, +0.0f, +1.0f, fig_names[fig_sel]); // Write name of selected figure
	SetTextAlign(hdc, TA_BOTTOM | TA_LEFT);
	draw_text(hdc, -1.0f, +1.0f, rotation_string); // Write name of selected figure
	SetTextAlign(hdc, TA_BOTTOM | TA_RIGHT);
	draw_text(hdc, +1.0f, +1.0f, projection_mode == PROJECTION_ORTHO ? _T("Параллельная") : _T("Центральная")); // Write name of selected figure
}
// Callback for WM_LBUTTONDOWN event
void CALLBACK on_click(HWND hWnd, POINT pt){
	if(++fig_sel == fig_count) // Loop selection
		fig_sel = 0;
	RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE); // Call WM_PAINT
}
// Callback for WM_DESTROY event
void CALLBACK on_destroy(HWND hWnd){
}
// Callback for WM_COMMAND event
void CALLBACK on_command(HWND hWnd, WORD wControl, WORD wCode, HWND hControl){
	if(wCode == BN_CLICKED){
		for(int i = 0; i < fig_count; ++i)
			if(hControl == btn_figures[i]){
				fig_sel = i;
				RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE); // Call WM_PAINT
				return;
			}
		for(int i = 0; i < 10; ++i)
			if(hControl == buttons[i]){
				if(i == 0) rotation = make_vec(0, 0, 320);
				if(i == 1) rotation.x = loop_angle(rotation.x + 10);
				if(i == 2) rotation.x = loop_angle(rotation.x - 10);
				if(i == 3) rotation.y = loop_angle(rotation.y + 10);
				if(i == 4) rotation.y = loop_angle(rotation.y - 10);
				if(i == 5) rotation.z = loop_angle(rotation.z + 10);
				if(i == 6) rotation.z = loop_angle(rotation.z - 10);
				if(i == 7) animation = animation ? FALSE : TRUE;
				if(i == 8) fill = fill ? FALSE : TRUE;
				if(i == 9) projection_mode = projection_mode == PROJECTION_ORTHO ? PROJECTION_PERSPECTIVE : PROJECTION_ORTHO;
				RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE); // Call WM_PAINT
				return;
			}
	}
}
void CALLBACK on_timer(LPARAM lParam){
	HWND hWnd = lParam;
	if(animation)
		rotation.y = loop_angle(rotation.y - 2);
	_stprintf_s(rotation_string, 32, _T("%3.0f, %3.0f, %3.0f"), rotation.x, rotation.y, rotation.z);
	InvalidateRect(hWnd, NULL, FALSE);
}

HWND*init_buttons(HWND window){
	HINSTANCE hInstance = GetWindowLongPtr(window, GWLP_HINSTANCE);
	HWND*btn_figs = malloc(sizeof(HWND)*fig_count);
	for(int i = 0; i < fig_count; ++i)
		btn_figs[i] = CreateWindow(
			_T("Button"),
			fig_names[i],
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
			WINDRAW_SIZE - 100, i * 22,
			100, 22,
			window, NULL,
			hInstance, NULL
		);
	buttons[0] = CreateWindow(
		_T("Button"),
		_T("Сброс"),
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		WINDRAW_SIZE - 100, (fig_count + 1) * 22,
		100, 22,
		window, NULL,
		hInstance, NULL
	);
	buttons[1] = CreateWindow(
		_T("Button"),
		_T("+X"),
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		WINDRAW_SIZE - 100, (fig_count + 2) * 22,
		50, 22,
		window, NULL,
		hInstance, NULL
	);
	buttons[2] = CreateWindow(
		_T("Button"),
		_T("-X"),
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		WINDRAW_SIZE - 50, (fig_count + 2) * 22,
		50, 22,
		window, NULL,
		hInstance, NULL
	);
	buttons[3] = CreateWindow(
		_T("Button"),
		_T("+Y"),
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		WINDRAW_SIZE - 100, (fig_count + 3) * 22,
		50, 22,
		window, NULL,
		hInstance, NULL
	);
	buttons[4] = CreateWindow(
		_T("Button"),
		_T("-Y"),
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		WINDRAW_SIZE - 50, (fig_count + 3) * 22,
		50, 22,
		window, NULL,
		hInstance, NULL
	);
	buttons[5] = CreateWindow(
		_T("Button"),
		_T("+Z"),
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		WINDRAW_SIZE - 100, (fig_count + 4) * 22,
		50, 22,
		window, NULL,
		hInstance, NULL
	);
	buttons[6] = CreateWindow(
		_T("Button"),
		_T("-Z"),
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		WINDRAW_SIZE - 50, (fig_count + 4) * 22,
		50, 22,
		window, NULL,
		hInstance, NULL
	);
	buttons[7] = CreateWindow(
		_T("Button"),
		_T("Анимация"),
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		WINDRAW_SIZE - 100, (fig_count + 6) * 22,
		100, 22,
		window, NULL,
		hInstance, NULL
	);
	buttons[8] = CreateWindow(
		_T("Button"),
		_T("Заливка"),
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		WINDRAW_SIZE - 100, (fig_count + 7) * 22,
		100, 22,
		window, NULL,
		hInstance, NULL
	);
	buttons[9] = CreateWindow(
		_T("Button"),
		_T("Проекция"),
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		WINDRAW_SIZE - 100, (fig_count + 8) * 22,
		100, 22,
		window, NULL,
		hInstance, NULL
	);

	return btn_figs;
}

void hide_window(HWND hWnd){
	LONG style = GetWindowLongPtr(hWnd, GWL_STYLE);
	style &= ~(WS_VISIBLE);
	style |= WS_EX_TOOLWINDOW;
	style &= ~WS_EX_APPWINDOW;
	ShowWindow(hWnd, SW_HIDE);
	SetWindowLongPtr(hWnd, GWL_STYLE, style);
	ShowWindow(hWnd, SW_SHOW);
	ShowWindow(hWnd, SW_HIDE);
}

int main(int argc, char**argv){
	rotation = make_vec(0, 0, 320);
	rotation_string = calloc(32, sizeof(TCHAR));
	init_figures(&figures, &fig_names, &fig_count); // Initialize figures
	// Transparent pen to hide faces' borders
	//pen = create_pen(make_rgb(0, 0, 0), PS_NULL);
	pen = create_pen(make_rgb(0.0f, 0.0f, 0.0f), PS_SOLID);
	// Brush to fill faces (#808080)
	brush = create_brush(make_rgb(0.5f, 0.5f, 0.5f));
	callbacks cb; // Setup callbacks
	cb.cb_paint = on_paint;
	cb.cb_click = on_click;
	cb.cb_destroy = on_destroy;
	cb.cb_command = on_command;
	HWND hWnd = init_window(cb); // Init graphics window
	btn_figures = init_buttons(hWnd); // Initialize buttons
	timer_start(100, on_timer, hWnd);
	UpdateWindow(hWnd);
	hide_window(GetConsoleWindow());
	window_loop(hWnd); // Start window event pump

	timer_stop(timer);
	DeleteObject(pen); // Destroy pen and brush
	DeleteObject(brush);
	free(btn_figures);
	for(int i = 0; i < fig_count; ++i)
		free(figures[i].faces);
	free(figures);
	free(fig_names);
	free(rotation_string);
	_tputs(_T("end"));
}
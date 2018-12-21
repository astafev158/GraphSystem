#include "settings_window.h"
#include <tchar.h>

/// �������������� ��������� ��� ������ ��������� ������
typedef struct {
	settings_window_click_callback callback;
	LPARAM lParam;
} WIN_USERDATA;

/// ��������� �������� ����
LRESULT CALLBACK wnd_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_CREATE) {
		LPCREATESTRUCT create = lParam; // ���������� ���������� ��������
		SetWindowLongPtr(hWnd, GWLP_USERDATA, create->lpCreateParams);// ��������� �������� � userdata
	}
	if (uMsg == WM_COMMAND && HIWORD(wParam) == BN_CLICKED) { // ������ ������
		WIN_USERDATA*ud = GetWindowLongPtr(hWnd, GWLP_USERDATA); // ���������� ��������� ������ �� ���������������� ������
		if (ud && ud->callback) // ���������, ���������� �� ��
			ud->callback(hWnd, lParam, ud->lParam);// ����� ��������� ������
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

/// Creates window of size with callback
HWND create_window(settings_window_click_callback callback, LPARAM lParam, int w, int h) {
	HINSTANCE hInstance = GetWindowLongPtr(GetConsoleWindow(), GWLP_HINSTANCE); // ���������� ������� � ���� �������
	LPCTSTR class_name = _T("class_name");
	WNDCLASS class; // ������� ����� ���� �� ���������
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
	RegisterClass(&class); // ����������� ���� ������
	WIN_USERDATA*ud = malloc(sizeof(WIN_USERDATA)); // ������� ��������� ������ ��������� ������
	ud->callback = callback;
	ud->lParam = lParam;
	HWND hWnd = CreateWindow( // ������� ����
		class_name,
		_T(""),
		WS_SYSMENU | WS_CAPTION | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		w, h,
		NULL, NULL,
		hInstance, ud
	);
	return hWnd; // ���������� ���� ����������
}

/// ��������� ������� ���������� ������ � ����
HWND add_button(HWND hWnd, int x, int y, int w, int h, LPCTSTR text) {
	return CreateWindow( // ������� ������ ���������� � ���������� � �������� ��������� ����
		_T("Button"), text,
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		x, y, w, h, hWnd, NULL,
		GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL
	);
}
/// ��������� ��������� ������� ���������� � ����
HWND add_text(HWND hWnd, int x, int y, int w, int h, LPCTSTR text) {
	return CreateWindow(		// ������� ��������� ������� ���������� � ���������� � �������� ��������� ����
		_T("Static"), text,
		WS_VISIBLE | WS_CHILD | SS_CENTER,
		x, y, w, h, hWnd, NULL,
		GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL
	);
}
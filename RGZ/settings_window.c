#include "settings_window.h"
#include <tchar.h>

/// Дополнительная структура для данных обратного вызова
typedef struct {
	settings_window_click_callback callback;
	LPARAM lParam;
} WIN_USERDATA;

/// Процедура главного окна
LRESULT CALLBACK wnd_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_CREATE) {
		LPCREATESTRUCT create = lParam; // Извлечение параметров создания
		SetWindowLongPtr(hWnd, GWLP_USERDATA, create->lpCreateParams);// Сохраняем параметр в userdata
	}
	if (uMsg == WM_COMMAND && HIWORD(wParam) == BN_CLICKED) { // Кнопка нажата
		WIN_USERDATA*ud = GetWindowLongPtr(hWnd, GWLP_USERDATA); // Извлечение обратного вызова из пользовательских данных
		if (ud && ud->callback) // Проверяем, установлен ли он
			ud->callback(hWnd, lParam, ud->lParam);// Вызов обратного вызова
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

/// Creates window of size with callback
HWND create_window(settings_window_click_callback callback, LPARAM lParam, int w, int h) {
	HINSTANCE hInstance = GetWindowLongPtr(GetConsoleWindow(), GWLP_HINSTANCE); // Извлечение позиции в окне консоли
	LPCTSTR class_name = _T("class_name");
	WNDCLASS class; // Создать класс окна по умолчанию
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
	RegisterClass(&class); // Регистрация окна класса
	WIN_USERDATA*ud = malloc(sizeof(WIN_USERDATA)); // Создать структуру данных обратного вызова
	ud->callback = callback;
	ud->lParam = lParam;
	HWND hWnd = CreateWindow( // Создать окно
		class_name,
		_T(""),
		WS_SYSMENU | WS_CAPTION | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		w, h,
		NULL, NULL,
		hInstance, ud
	);
	return hWnd; // Возвращаем свой дескриптор
}

/// Добавляет элемент управления кнопки в окно
HWND add_button(HWND hWnd, int x, int y, int w, int h, LPCTSTR text) {
	return CreateWindow( // Создать кнопку управления и установить в качестве дочернего окна
		_T("Button"), text,
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		x, y, w, h, hWnd, NULL,
		GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL
	);
}
/// Добавляет текстовый элемент управления в окно
HWND add_text(HWND hWnd, int x, int y, int w, int h, LPCTSTR text) {
	return CreateWindow(		// Создать текстовый элемент управления и установить в качестве дочернего окна
		_T("Static"), text,
		WS_VISIBLE | WS_CHILD | SS_CENTER,
		x, y, w, h, hWnd, NULL,
		GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL
	);
}
#pragma once
#ifndef SETTINGS_WINDOW_H
#define SETTINGS_WINDOW_H
#include <Windows.h>

/// Кнопка обратного вызова
typedef void (CALLBACK*settings_window_click_callback)(HWND hWnd, HWND hControl, LPARAM lParam);
/// Создает окно размера с обратным вызовом
HWND create_window(settings_window_click_callback callback, LPARAM lParam, int w, int h);
// Добавляет элемент управления кнопки в окно и возвращает его обработчик
HWND add_button(HWND hWnd, int x, int y, int w, int h, LPCTSTR text);
/// Добавляет текстовый элемент управления в окно и возвращает его обработчик
HWND add_text(HWND hWnd, int x, int y, int w, int h, LPCTSTR text);

#endif
#pragma once
#ifndef SETTINGS_WINDOW_H
#define SETTINGS_WINDOW_H
#include <Windows.h>

/// ������ ��������� ������
typedef void (CALLBACK*settings_window_click_callback)(HWND hWnd, HWND hControl, LPARAM lParam);
/// ������� ���� ������� � �������� �������
HWND create_window(settings_window_click_callback callback, LPARAM lParam, int w, int h);
// ��������� ������� ���������� ������ � ���� � ���������� ��� ����������
HWND add_button(HWND hWnd, int x, int y, int w, int h, LPCTSTR text);
/// ��������� ��������� ������� ���������� � ���� � ���������� ��� ����������
HWND add_text(HWND hWnd, int x, int y, int w, int h, LPCTSTR text);

#endif
#pragma once
#ifndef WINTIMER_H
#define WINTIMER_H
#include <Windows.h>

typedef void(CALLBACK*timer_callback)(LPVOID);
typedef struct WINTIMER WINTIMER,*LPWINTIMER;
LPWINTIMER timer_start(DWORD interval, timer_callback callback, LPVOID lParam);
void timer_stop(LPWINTIMER timer);

#endif
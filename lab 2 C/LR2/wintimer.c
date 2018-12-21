#include "wintimer.h"

struct WINTIMER {
	timer_callback callback;
	LPVOID param;
	DWORD interval;
	BOOL run;
};

DWORD WINAPI timer_func(LPWINTIMER timer) {
	while (timer->run) {
		timer->callback(timer->param);
		Sleep(timer->interval);
	}
	HeapFree(GetProcessHeap(), 0, timer);
	return 0;
}

LPWINTIMER timer_start(DWORD interval, timer_callback callback, LPVOID lParam) {
	LPWINTIMER timer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(WINTIMER));
	timer->interval = interval;
	timer->callback = callback;
	timer->param = lParam;
	timer->run = TRUE;
	HANDLE h = CreateThread(
		NULL, 0,
		timer_func,
		timer,
		0, NULL
	);
	if (h == NULL) {
		HeapFree(GetProcessHeap(), 0, timer);
		return NULL;
	}
	return timer;
}
void timer_stop(LPWINTIMER timer) {
	if(timer)
		timer->run = FALSE;
}
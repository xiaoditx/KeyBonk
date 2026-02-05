#ifndef MOUSE_HOOK_HPP
#define MOUSE_HOOK_HPP

#include <Windows.h>

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);

#endif // MOUSE_HOOK_HPP
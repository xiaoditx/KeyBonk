#include "../../include/hook/mouse_hook.hpp"

// 安装鼠标钩子
HHOOK InstallMouseHook(MouseHookProc* hookProc)
{
    return SetWindowsHookEx(WH_MOUSE_LL, hookProc, NULL, 0);
}

// 卸载鼠标钩子
BOOL UninstallMouseHook(HHOOK hHook)
{
    return UnhookWindowsHookEx(hHook);
}

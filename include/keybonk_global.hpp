// ===./include/keybonk_global.h===
// 全局变量和共享定义

#ifndef KEYBONK_GLOBAL_H
#define KEYBONK_GLOBAL_H

#include <windows.h>
#include <gdiplus.h>

// 全局变量声明
extern ULONG_PTR g_gdiplusToken;           // GDI+的token
extern HWND hwnd;                          // 主窗口句柄
extern HWND hwndAbout;                     // "关于"窗口句柄
extern Gdiplus::Image *g_pBackgroundImage; // 背景图片
extern bool Mute;                          // 是否静音
extern bool WindowPenetrate;               // 窗口穿透
extern NOTIFYICONDATA nid;                 // 任务栏通知区域图标状态
extern bool minimum;                       // 当前最小化状态
extern HINSTANCE C_hInstance;
extern int C_nCmdShow;
extern HRESULT hrMain; // 接受Windows函数的返回结果

#endif // KEYBONK_GLOBAL_H
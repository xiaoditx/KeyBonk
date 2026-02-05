// ===./include/global.hpp===
// 全局变量和共享定义

#ifndef GLOBAL_H
#define GLOBAL_H

#include <windows.h>

// 全局变量声明
extern ULONG_PTR g_gdiplusToken;       // GDI+的token
extern bool comInitialized;            // COM库是否初始化成功
extern wchar_t *fullIniFilePath;       // 配置文件完整路径
extern wchar_t *fullDebugFilePath;     // 日志文件完整路径
extern HWND hwnd;                      // 主窗口句柄
extern HWND hwndAbout;                 // "关于"窗口句柄
extern HWND hwndSetting;               // “设置”窗口句柄
extern bool Mute;                      // 键盘是否静音
extern bool MuteMouse;                 // 鼠标是否静音
extern bool WindowPenetrate;           // 窗口穿透
extern NOTIFYICONDATA nid;             // 任务栏通知区域图标状态
extern bool minimum;                   // 当前最小化状态
extern wchar_t audioLibPath[MAX_PATH]; // 音频库位置
extern HHOOK KeyboardHook;             // 钩子句柄
extern HHOOK MouseHook;                // 钩子句柄
extern HBITMAP hBmp;                   // 存储背景图片的位图
extern HDC hdcScreen;                  // 主窗口屏幕DC
extern HDC memDC;                      // 主窗口内存DC
extern HBITMAP hOldBmp;                // 主窗口内存DC默认位图
extern HINSTANCE C_hInstance;
extern int C_nCmdShow;
extern HRESULT hrMain; // 接受Windows函数的返回结果

/**
 * @brief 统一释放需要释放的全局资源
 */
void releaseGlobalResources();
#endif // GLOBAL_H
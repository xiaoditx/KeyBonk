// === ./main.cpp ===
// 主程序存放的文件
// 包含消息循环，作为入口

#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <gdiplus.h>
#include <shellapi.h>
#include "../include/keybonk_global.h"
#include "../include/window_manager.h"
#include "../include/keyboard_hook.h"
#include "../resource/resources.h"

// 主程序
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow){
    
    // 保存hInstance到全局
    C_hInstance = hInstance;

    // 初始化COM库
    hrMain = CoInitializeEx(NULL,
        // 单元线程，禁用OLE1
        COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE
    );

    if(FAILED(hrMain)){
        MessageBoxExW(
            NULL,L"错误：00001，初始化COM库时发生异常，请检查系统相关文件是否完好",
            L"KB - 运行时发生错误",MB_OK|MB_ICONEXCLAMATION,0
        ); // 消息框提示出错
        return 0;
    }

    // 注册窗口类
    const wchar_t CLASS_NAME[]  = L"KeyBonk主窗口";
    WNDCLASS wc = { }; // 用0初始化整个WindowClass
    wc.lpfnWndProc = WindowProc; // 指定WindowProc函数
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME; // 窗口类名称
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); // 图标（其实意义不大）
    RegisterClass(&wc); // 注册

    // 创建窗口
    hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW, // 支持透明，设置置顶，隐藏软件图标
        CLASS_NAME, // 窗口类
        L"KeyBonk主窗口", // 窗口文本
        WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN, // 窗口风格
		// 大小+位置
        100, 100,200,200,
        NULL,       // 父窗口   
        NULL,       // 菜单
        hInstance,  // 示例句柄
        NULL        // 附带的软件数据
	);

    // 创建失败则提示并返回，结束运行
    if (hwnd == NULL){
        MessageBoxExW(
            NULL,L"错误：00002，创建窗口时发生异常，请检查系统各项设置是否正常",
            L"KB - 运行时发生错误",MB_OK|MB_ICONEXCLAMATION,0
        ); // 消息框提示出错
        return 0;
    }

    // 设置透明
    COLORREF crKey = 13217535; // 偏粉红色的颜色
    SetLayeredWindowAttributes(hwnd, crKey, 0, LWA_COLORKEY);

    ShowWindow(hwnd, nCmdShow); //展示窗口

    // 初始化GDI+
    Gdiplus::GpStatus GDIpStatus;
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    GDIpStatus = Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);
	if(GDIpStatus != Gdiplus::Ok){
        MessageBoxExW(
            NULL,L"错误：00003，初始化GDI+库时发生异常",
            L"KB - 运行时发生错误",MB_OK|MB_ICONEXCLAMATION,0
        ); // 消息框提示出错
        return 0;
    }
    // 加载背景图片
    g_pBackgroundImage = new Gdiplus::Image(L"./resource/background.png");

    // 安装钩子
    HHOOK KeyboardHook = NULL;
    KeyboardHook = SetWindowsHookExW(
        WH_KEYBOARD_LL, // 低级键盘钩子
        // 似乎也能用WH_KEYBOARD，但低级钩子用起来更稳定、简单些
        LowLevelKeyboardProc, // 传递回调函数地址
        GetModuleHandle(NULL),
        0
    );
    if(KeyboardHook == NULL){
        MessageBoxExW(
            NULL,L"错误：00004，键盘钩子安装失败，请检查杀毒软件是否关闭",
            L"KB - 运行时发生错误",MB_OK|MB_ICONEXCLAMATION,0
        ); // 消息框提示出错
    }

    // 消息循环
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
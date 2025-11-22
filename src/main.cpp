// === ./main.cpp ===
// 主程序存放的文件，包含大量全局相关的信息，调度初始化信息等
// 包含消息循环，程序的大多数流程都在此发生

#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <gdiplus.h>
#include <wchar.h>
#include <filesystem>
#include <string>

// 后期版本会放入类的内容
ULONG_PTR g_gdiplusToken; // GDI+的token
HWND hwnd;
Gdiplus::Image* g_pBackgroundImage;


// 各种向前声明
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

// 主程序
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow){

    // 初始化COM库
    CoInitialize(NULL);

    // 注册窗口类
    const wchar_t CLASS_NAME[]  = L"Sample Window Class";
    WNDCLASS wc = { };
    wc.lpfnWndProc   = WindowProc; // 指定WindowProc函数
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    // 创建窗口
    hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST, // 支持透明，设置置顶
        CLASS_NAME, // 窗口类
        L"Learn to Program Windows", // 窗口文本
        WS_POPUP | WS_VISIBLE, // 窗口风格
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
            NULL,L"错误：00001，创建窗口时发生异常，请检查系统各项设置是否正常",
            L"KB - 运行时发生错误",MB_OK|MB_ICONEXCLAMATION,0
        ); // 消息框提示出错
        return 0;
    }

    COLORREF crKey = 13217535;
    SetLayeredWindowAttributes(hwnd, crKey, 0, LWA_COLORKEY);

    ShowWindow(hwnd, nCmdShow); //展示窗口

    // 初始化GDI+
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);
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

    // 消息循环
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

// 判断文件是否存在
bool FileExists(const wchar_t* rawPath){// 接收 C 风格字符串
    std::wstring_view pathView{ rawPath };// 或者 std::wstring path{ rawPath };
    return std::filesystem::exists(pathView);// 支持 std::wstring_view/wstring/const wchar_t*
}

// 消息处理
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch (uMsg){
		case WM_DESTROY:
            // 资源释放
            // pControls->Release();
            // pPlayer->Release();
            delete g_pBackgroundImage; // 释放背景图片（真的有必要吗🤔）
            Gdiplus::GdiplusShutdown(g_gdiplusToken); // 关闭GDI库
            CoUninitialize(); // 关闭COM库
        	PostQuitMessage(0);
        return 0;

        case WM_PAINT:{
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                Gdiplus::Graphics graphics(hdc);

                // 获取客户区大小
                RECT clientRect;
                GetClientRect(hwnd, &clientRect);
                int windowWidth = clientRect.right - clientRect.left;
                int windowHeight = clientRect.bottom - clientRect.top;

                // 将图片绘制到整个窗口客户区
                if (g_pBackgroundImage) {
                    graphics.DrawImage(g_pBackgroundImage, 0, 0, windowWidth, windowHeight);
                }
                EndPaint(hwnd, &ps);
			}
			return 0;
        case WM_SIZE:{
                // 窗口大小改变时强制重绘
                InvalidateRect(hwnd, NULL, TRUE);
                return 0;
            }
        case WM_NCHITTEST: {
            LRESULT hit = DefWindowProc(hwnd, uMsg, wParam, lParam);
            // 让客户区可拖动
            if (hit == HTCLIENT) hit = HTCAPTION;
            return hit;
        }
        default :return DefWindowProcW(hwnd,uMsg,wParam,lParam);
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// 低级键盘钩子的回调函数
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* keyInfo = (KBDLLHOOKSTRUCT*)lParam;
        // 判断是否为按键按下事件
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            DWORD vkCode = keyInfo->vkCode;
            wchar_t szPath[MAX_PATH];
            swprintf_s(szPath,
                    _countof(szPath),
                    L"./resource/audios/%lu.wav",   // 格式串
                    vkCode);   // 对应的数字
            if(FileExists(szPath))
                PlaySoundW(szPath, NULL, SND_FILENAME | SND_ASYNC);
        }
    }
    // 按照规定需要将事件传递给下一个钩子或系统
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}
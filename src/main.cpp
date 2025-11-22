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
#include <shellapi.h>
#include "../resource/resources.h"
#include <winver.h>

// 全局变量
ULONG_PTR g_gdiplusToken; // GDI+的token
HWND hwnd; // 窗口句柄
Gdiplus::Image* g_pBackgroundImage; // 背景图片
bool Mute= false; // 是否静音
bool WindowPenetrate = false; // 窗口穿透
NOTIFYICONDATA nid = {}; // 任务栏通知区域图标状态
bool minimum = false;
HINSTANCE C_hInstance;

// 各种向前声明
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam); // 消息处理
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam); // 钩子
void RemoveTrayIcon();
BOOL AddTrayIcon(HWND hWnd);

// 简单功能函数

template<typename T>
void safeRelease(T** resourcePointer){
    // 检查是否为空
    if(*resourcePointer){
        // 释放资源
        (*resourcePointer)->Release();
        // 设置为空
        resourcePointer = NULL;
    }
}

// 主程序
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow){
    
    // 保存hInstance到全局
    C_hInstance = hInstance;

    // 初始化COM库
    CoInitializeEx(NULL,
        COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE
    );

    // 注册窗口类
    const wchar_t CLASS_NAME[]  = L"KeyBonk主窗口";
    WNDCLASS wc = { }; // 用0初始化整个WindowClass
    wc.lpfnWndProc = WindowProc; // 指定WindowProc函数
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
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

// 设置窗口穿透
bool SetWindowMouseTransparent(HWND hWnd, bool enable)
{
    if (!hWnd || !IsWindow(hWnd))
        return false;
    
    LONG_PTR exStyle = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
    
    if (enable)
        exStyle |= WS_EX_TRANSPARENT;
    else
        exStyle &= ~WS_EX_TRANSPARENT;
    
    SetWindowLongPtr(hWnd, GWL_EXSTYLE, exStyle);
    
    // 刷新窗口
    SetWindowPos(hWnd, NULL, 0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    
    return true;
}

// 消息处理
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch (uMsg){
		case WM_DESTROY:
            if (g_pBackgroundImage){
                delete g_pBackgroundImage; // 释放背景图片（真的有必要吗🤔）
                g_pBackgroundImage = NULL;
            }
            RemoveTrayIcon();
            Gdiplus::GdiplusShutdown(g_gdiplusToken); // 关闭GDI库
            CoUninitialize(); // 关闭COM库
        	PostQuitMessage(0);
        return 0;

        case WM_RBUTTONDOWN:
        {
            // 显示右键菜单
            HMENU hMenu = LoadMenu(C_hInstance, MAKEINTRESOURCE(IDR_CONTEXT_MENU));
            HMENU hSubMenu = GetSubMenu(hMenu, 0);
            
            // 设置菜单项的初始选中状态
            UINT uWindowPenetrateState = WindowPenetrate ? MF_CHECKED : MF_UNCHECKED;
            UINT uMuteState = Mute ? MF_CHECKED : MF_UNCHECKED;
            
            CheckMenuItem(hSubMenu, IDM_WINDOW_PENETRATE, 
                        MF_BYCOMMAND | uWindowPenetrateState);
            CheckMenuItem(hSubMenu, IDM_MUTE, 
                        MF_BYCOMMAND | uMuteState);
            
            POINT pt = { LOWORD(lParam), HIWORD(lParam) };
            ClientToScreen(hwnd, &pt);
            
            // 显示右键菜单
            TrackPopupMenu(hSubMenu, 
                        TPM_RIGHTBUTTON | TPM_LEFTALIGN,
                        pt.x, pt.y, 0, hwnd, NULL);
            DestroyMenu(hMenu);
            return 0;
        }

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
            if (hit == HTCLIENT && (GetAsyncKeyState(VK_LBUTTON) & 0x8000)) hit = HTCAPTION;
            return hit;
        }
        case WM_COMMAND:
            switch (LOWORD(wParam)){
                case IDM_WINDOW_PENETRATE:
                    WindowPenetrate = (!WindowPenetrate);
                    SetWindowMouseTransparent(hwnd,WindowPenetrate);
                    break;
                case IDM_MUTE:
                    Mute=(!Mute);
                    break;
                case IDM_EXIT:
                    PostQuitMessage(0);
                    break;
                case IDM_MINIMUM:
                    minimum =! minimum;
                    if(minimum){
                        ShowWindow(hwnd, SW_MINIMIZE);
                    }else{
                        ShowWindow(hwnd, SW_RESTORE);
                    }
                    break;
                case IDM_SETTINGS:
                    MessageBoxExW(
                        NULL,L"还没有开发呢",
                        L"嘻嘻",MB_OK|MB_ICONEXCLAMATION,0
                    );
                    break;
                case IDM_ABOUT:
                    MessageBoxExW(
                        NULL,L"还没有开发呢",
                        L"嘻嘻",MB_OK|MB_ICONEXCLAMATION,0
                    ); // 消息框提示出错
                    break;
            }
            return 0;
        case WM_CREATE:
            {
                AddTrayIcon(hwnd);
            }
            break;
            
        case (WM_USER + 1):
            if (lParam == WM_RBUTTONDOWN)
            {
                // 显示右键菜单
                HMENU hMenu = LoadMenu(C_hInstance, MAKEINTRESOURCE(IDR_CONTEXT_MENU));
                HMENU hSubMenu = GetSubMenu(hMenu, 0);
                
                // 设置菜单项的初始选中状态
                UINT uWindowPenetrateState = WindowPenetrate ? MF_CHECKED : MF_UNCHECKED;
                UINT uMuteState = Mute ? MF_CHECKED : MF_UNCHECKED;
                UINT uMinimumState = minimum ? MF_CHECKED : MF_UNCHECKED;
                
                CheckMenuItem(hSubMenu, IDM_WINDOW_PENETRATE, 
                            MF_BYCOMMAND | uWindowPenetrateState);
                CheckMenuItem(hSubMenu, IDM_MUTE, 
                            MF_BYCOMMAND | uMuteState);
                CheckMenuItem(hSubMenu, IDM_MINIMUM, 
                            MF_BYCOMMAND | uMinimumState);
                
                POINT pt;
                GetCursorPos(&pt);  // 获取当前鼠标的屏幕坐标
                
                // 确保窗口在前台，这样点击其他地方时会正确关闭菜单
                SetForegroundWindow(hwnd);

                // 显示右键菜单
                TrackPopupMenu(hSubMenu, 
                            TPM_RIGHTBUTTON | TPM_LEFTALIGN,
                            pt.x, pt.y, 0, hwnd, NULL);

                PostMessage(hwnd, WM_NULL, 0, 0);
                
                DestroyMenu(hMenu);
            }
            else if (lParam == WM_LBUTTONDBLCLK)
            {
                // 双击左键显示窗口
                ShowWindow(hwnd, SW_SHOW);
                SetForegroundWindow(hwnd);
            }
            break;
        default :
            return DefWindowProcW(hwnd,uMsg,wParam,lParam);
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// 低级键盘钩子的回调函数
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* keyInfo = (KBDLLHOOKSTRUCT*)lParam;
        // 判断是否为按键按下事件
        if ((wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) and not Mute) {
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

// 添加托盘图标函数
BOOL AddTrayIcon(HWND hWnd)
{
    // 从资源加载图标
    HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MY_ICON));
    if (!hIcon)
        return FALSE;

    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = IDI_MY_ICON;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = (WM_USER + 1);
    nid.hIcon = hIcon;
    
    // 设置提示文本
    lstrcpy(nid.szTip, TEXT("KeyBonk"));
    
    return Shell_NotifyIcon(NIM_ADD, &nid);
}


// 删除托盘图标函数
void RemoveTrayIcon()
{
    Shell_NotifyIcon(NIM_DELETE, &nid);
}
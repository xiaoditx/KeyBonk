// window_manager.cpp
// 窗口管理相关功能实现

#include <windows.h>
#include <gdiplus.h>
#include <shellapi.h>
#include <winver.h>
#include "../include/window_manager.h"
#include "../resource/resources.h"

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
                delete g_pBackgroundImage; // 释放背景图片
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
                    );
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
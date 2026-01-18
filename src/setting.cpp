// ===./src/setting.cpp===
// 设置窗口相关功能实现

#ifndef UNICODE // UNICODE宏可以让Windows函数自动的匹配到W版本
#define UNICODE
#endif

#include <windows.h>
#include <gdiplus.h>
#include <shellapi.h>
#include <winver.h>
#include <string>
#include "../include/setting.hpp"
#include "../include/keybonk_global.hpp"
#include "../resource/resources.hpp"

// 打开“设置”窗口
void settingWindowOpen()
{
	// 检查设置窗口是否已经存在（窗口只能存在一次）
	if (hwndSetting != NULL && IsWindow(hwndSetting))
	{
		// 检查窗口是否被最小化或隐藏
		if (IsIconic(hwndSetting))
		{
			// 恢复窗口
			ShowWindow(hwndSetting, SW_RESTORE);
		}
		else if (!IsWindowVisible(hwndSetting))
		{
			// 显示窗口
			ShowWindow(hwndSetting, SW_SHOW);
		}
		// 激活窗口并设置为前台
		SetForegroundWindow(hwndSetting);
		return;
	}

	// 注册窗口类
	const wchar_t CLASS_NAME[] = L"KeyBonk“设置”窗口";
	WNDCLASSEX wc = {};					 // 用0初始化整个WindowClass
	wc.cbSize = sizeof(WNDCLASSEX);		 // 设置结构体大小
	wc.lpfnWndProc = WindowProc_setting; // 指定WindowProc_about函数
	wc.hInstance = C_hInstance;
	wc.lpszClassName = CLASS_NAME; // 窗口类名称
	wc.hIcon = (HICON)LoadImage(C_hInstance, MAKEINTRESOURCE(IDI_MY_ICON), IMAGE_ICON, 64, 64, 0);
	wc.hIconSm = (HICON)LoadImage(C_hInstance, MAKEINTRESOURCE(IDI_MY_ICON), IMAGE_ICON, 64, 64, 0); // 小图标（窗口标题栏）
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	RegisterClassEx(&wc); // 注册

	hwndSetting = CreateWindowExW(
		WS_EX_APPWINDOW, CLASS_NAME, L"设置",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 600, 400,
		NULL, NULL, C_hInstance, NULL);

	// 创建失败则提示并返回，结束运行
	if (hwndSetting == NULL)
	{
		MessageBoxExW(
			NULL, L"错误：00002，创建窗口时发生异常，请检查系统各项设置是否正常",
			L"KB - 运行时发生错误", MB_OK | MB_ICONEXCLAMATION, 0); // 消息框提示出错
	}
	ShowWindow(hwndSetting, C_nCmdShow);
	// UpdateWindow(hwndAbout);
}

#define IDC_LABEL1 1008

// 设置窗口消息处理
LRESULT CALLBACK WindowProc_setting(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE: // 绘制图形界面
	{
		// 获取客户区大小
		RECT rect;
		GetClientRect(hwnd, &rect);
		int clientWidth = rect.right - rect.left;					// 客户区宽度
		[[maybe_unused]] int clientHeight = rect.bottom - rect.top; // 客户区高度

		// 创建组件
		// 标签1
		CreateWindowExW(0, L"STATIC", L"选择的音频库：",
						WS_CHILD | WS_VISIBLE | SS_LEFT | SS_NOPREFIX,
						130, 20, clientWidth - 150, 30,
						hwnd, (HMENU)IDC_LABEL1, C_hInstance, NULL);

		// 设置字体（微软雅黑，24号）
		// 字体变量
		HFONT hNormalFont = CreateFontW(24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
										DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
										CLEARTYPE_QUALITY, VARIABLE_PITCH, L"微软雅黑");
		// 应用字体
		SendMessageW(GetDlgItem(hwnd, IDC_LABEL1), WM_SETFONT, (WPARAM)hNormalFont, TRUE);
		break;
	}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}
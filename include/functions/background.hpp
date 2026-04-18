#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP
#include <windows.h>
#include <gdiplus.h>
#include <memory>
#include <string>

#include "yumo_except.hpp"
#include "functions/files.hpp"

namespace keybonk
{
    class background
    {
    private:
        HWND hwnd_;                // 主窗口句柄
        const wchar_t *img_path_;  // 图片库的背景图片路径
        Gdiplus::Bitmap *pBitmap_; // GDI+位图对象
        HBITMAP hBmp_;             // 背景图片的位图句柄
        HDC hdcScreen_;            // 屏幕DC
        HDC memDC_;                // 内存DC
        HBITMAP hOldBmp_;          // 内存DC默认位图
        int nCmdShow_;
        void setImgSize_();
        void loadImg_(const wchar_t *path);

    public:
        // 构造函数
        explicit background() : hwnd_(NULL), img_path_(nullptr), pBitmap_(nullptr), hBmp_(NULL), nCmdShow_(SW_HIDE) {};
        explicit background(HWND hwnd, const wchar_t *img_path, int nCmdShow)
            : hwnd_(hwnd), img_path_(img_path),
              pBitmap_(nullptr), hBmp_(NULL), nCmdShow_(nCmdShow)
        {
            resetToDefault();
        };

        background(const background &) = delete;
        background &operator=(const background &) = delete;

        void resetImgPath(const wchar_t *newPath)
        {
            img_path_ = newPath;
        }
        void resetBackground(DWORD keyCode, bool isMouse);
        void resetToDefault();
        ~background()
        {
            // 清理DC和位图
            if (memDC_ != nullptr && hOldBmp_ != nullptr)
            {
                SelectObject(memDC_, hOldBmp_); // 选出自定义位图
                DeleteDC(memDC_);
            }
            if (hdcScreen_ != nullptr)
            {
                ReleaseDC(hwnd_, hdcScreen_);
            }
            if (hBmp_ != nullptr)
            {
                DeleteObject(hBmp_);
            }
            // 保险起见 ↓
            if (pBitmap_ != nullptr)
            {
                delete pBitmap_;
            }
        }
    };

}

#endif // BACKGROUND_HPP
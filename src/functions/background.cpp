#include "functions/background.hpp"

namespace keybonk
{
    // todo 某些函数应该是内联的

    void background::resetBackground(DWORD keyCode, bool isMouse)
    {
        auto tmp_path = std::make_unique<wchar_t[]>(MAX_PATH);
        swprintf_s(tmp_path.get(),
                   MAX_PATH,
                   L"%ls\\%ls\\%lu.png",
                   img_path_,
                   isMouse ? L"mouse" : L"keyboard",
                   keyCode);
        this->loadImg_(tmp_path.get());
        this->setImgSize_();
    }

    void background::loadImg_(const wchar_t *path)
    {
        if (FileExists(path))
        {
            pBitmap_ = Gdiplus::Bitmap::FromFile(path);
            if (!pBitmap_ || pBitmap_->GetLastStatus() != Gdiplus::GpStatus::Ok)
            {
                // 图片加载失败，创建红色矩形作为替代
                pBitmap_ = new Gdiplus::Bitmap(160, 180);
                Gdiplus::Graphics g(pBitmap_);
                Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 0, 0));
                g.FillRectangle(&brush, 0, 0, 160, 180);
            }
        }
        else
        {
            // 抛出异常
            throw keybonk::exception(
                keybonk::exception::type::FileNotFound);
        }
    }

    void background::setImgSize_()
    {
        // 获取原始位图尺寸
        const int originalWidth = pBitmap_->GetWidth();
        const int originalHeight = pBitmap_->GetHeight();

        // 获取屏幕尺寸
        const int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        const int screenHeight = GetSystemMetrics(SM_CYSCREEN);

        // 判断尺寸是否合法，避免后续计算出问题
        if (!(originalWidth > 0 && originalHeight > 0) || !(screenWidth > 0 && screenHeight > 0))
        {
            throw keybonk::exception(
                keybonk::exception::type::InvalidInput);
        }

        const bool notPortrait = screenHeight <= screenWidth; // 判断是否竖屏

        // 计算最大允许尺寸
        const int maxWidth = screenWidth / (notPortrait ? 4 : 2);   // 屏幕宽的四分之一
        const int maxHeight = screenHeight / (notPortrait ? 2 : 4); // 屏幕高的二分之一

        // 计算缩放比例
        float widthRatio = (float)originalWidth / maxWidth;
        float heightRatio = (float)originalHeight / maxHeight;
        float scaleRatio = std::max(widthRatio, heightRatio);

        // 计算缩放后的尺寸
        int scaledWidth, scaledHeight;
        if (scaleRatio > 1.0f)
        {
            // 需要缩放
            scaledWidth = (int)(originalWidth / scaleRatio);
            scaledHeight = (int)(originalHeight / scaleRatio);
        }
        else
        {
            // 不需要缩放
            scaledWidth = originalWidth;
            scaledHeight = originalHeight;
        }

        // 创建缩放后的位图
        Gdiplus::Bitmap *scaledBitmap = new Gdiplus::Bitmap(scaledWidth, scaledHeight);
        Gdiplus::Graphics graphics(scaledBitmap);

        // 设置高质量缩放
        graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);

        // 绘制缩放后的图像
        graphics.DrawImage(pBitmap_, 0, 0, scaledWidth, scaledHeight);

        // 清理原始位图
        delete pBitmap_;
        pBitmap_ = scaledBitmap; // 使用缩放后的位图

        // 将GDI+位图转换为HBITMAP
        pBitmap_->Gdiplus::Bitmap::GetHBITMAP(
            Gdiplus::Color(0, 0, 0, 0),
            &hBmp_); // 透明背景

        delete pBitmap_;

        // 创建内存DC
        hdcScreen_ = GetDC(hwnd_);
        memDC_ = CreateCompatibleDC(hdcScreen_);
        hOldBmp_ = (HBITMAP)SelectObject(memDC_, hBmp_);

        // 使用UpdateLayeredWindow
        SIZE size = {scaledWidth, scaledHeight};
        POINT ptSrc = {0, 0};
        BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        UpdateLayeredWindow(hwnd_, hdcScreen_, NULL, &size, memDC_, &ptSrc, 0, &bf, ULW_ALPHA);

        ShowWindow(hwnd_, nCmdShow_);
        UpdateWindow(hwnd_);
    }

    void background::resetToDefault()
    {
        auto tmp_path = std::make_unique<wchar_t[]>(MAX_PATH);
        swprintf_s(tmp_path.get(),
                   MAX_PATH,
                   L"%ls\\background.png",
                   img_path_);
        this->loadImg_(tmp_path.get());
        this->setImgSize_();
    }

    background::~background()
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
}
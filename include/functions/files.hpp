#ifndef FILES_H
#define FILES_H

#include <wchar.h>

// 各种文件路径处理函数
bool GetExePath(wchar_t *outBuffer, size_t bufferSize);
bool GetExeDirectory(wchar_t *outBuffer, size_t bufferSize);
bool GetExeRelativePath(const wchar_t *rawPath, wchar_t *outBuffer, size_t bufferSize);
bool FileExists(const wchar_t *rawPath);

#endif // FILES_H

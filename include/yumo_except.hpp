#ifndef YUMO_EXCEPT_HPP
#define YUMO_EXCEPT_HPP

#include <string>

/**
 * @file yumo_except.hpp
 * @brief 定义了keybonk项目中使用的自定义异常类
 * 这个文件包含了一个简单的异常类，用于在项目中抛出和捕获特定类型的错误。通过使用这个自定义异常类，可以更清晰地表达错误的类型和原因，便于调试和维护。
 * @author XiaoDi
 */

namespace keybonk
{
    /**
     * @brief 自定义异常类
     *
     * 这个类定义了一个简单的异常类型，包含一个枚举类型来表示不同的错误类型，本类仅提供简单的错误信息，适用于catch块掌握详细信息的情况。
     * @note 目前定义了三种错误类型：FileNotFound（文件未找到）、InvalidInput（无效输入）和UnknownError（未知错误）。根据实际需要，可以扩展这个枚举类型以包含更多的错误类型。
     */
    class exception
    {
    public:
        enum class type
        {
            FileNotFound,
            InvalidInput,
            UnknownError
        };

    private:
        type type_;

    public:
        exception() = delete;
        exception(type t) : type_(t) {}
        type getType() const { return type_; }
        ~exception() = default;
    };

    /**
     * @brief 带有错误信息的自定义异常类
     *
     * 这个类继承自`keybonk::exception`类，增加了一个成员变量`message_`来存储错误信息。构造函数接受一个异常类型和一个错误信息字符串，并提供了一个what()方法来获取错误信息。
     * @note 这个类适用于需要在catch块中获取详细错误信息的情况，可以通过what()方法获取到具体的错误描述，便于调试和用户提示。
     */
    class exception_ex : public exception
    {
    private:
        std::wstring message_;

    public:
        exception_ex() = delete;
        exception_ex(exception::type t, const std::wstring &msg) : exception(t), message_(msg) {}
        const std::wstring &what() const noexcept { return message_; }
        ~exception_ex() = default;
    };
}
#endif // YUMO_EXCEPT_HPP
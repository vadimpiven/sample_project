#include <platform/text/text_converter.h>

#include <stdexcept>

#include <Windows.h>

namespace platform::text {

[[nodiscard]] std::wstring TextConverter::Utf8ToUtf16(const std::string & string)
{
    if (string.empty())
    {
        return {};
    }

    const auto expectedLength = ::MultiByteToWideChar(
            CP_UTF8, 0,
            string.data(), static_cast<int>(string.size()),
            nullptr, 0
    );
    if (expectedLength == 0)
    {
        throw std::runtime_error(
                "MultiByteToWideChar failed calculating size, error = " + std::to_string(GetLastError()));
    }

    auto result = std::wstring(static_cast<std::wstring::size_type>(expectedLength), '\0');

    const auto actualLength = ::MultiByteToWideChar(
            CP_UTF8, 0,
            string.data(), static_cast<int>(string.size()),
            result.data(), static_cast<int>(result.size())
    );
    if (actualLength != expectedLength)
    {
        throw std::runtime_error(
                "MultiByteToWideChar failed converting string, error = " + std::to_string(GetLastError()));
    }

    return result;
}

[[nodiscard]] std::string TextConverter::Utf16ToUtf8(const std::wstring & string)
{
    if (string.empty())
    {
        return {};
    }

    const auto expectedLength = ::WideCharToMultiByte(
        CP_UTF8, 0,
        string.data(), static_cast<int>(string.size()),
        nullptr, 0,
        nullptr, nullptr
    );
    if (expectedLength == 0)
    {
        throw std::runtime_error(
            "WideCharToMultiByte failed calculating size, error = " + std::to_string(GetLastError()));
    }

    auto result = std::string(static_cast<std::wstring::size_type>(expectedLength), '\0');

    const auto actualLength = ::WideCharToMultiByte(
        CP_UTF8, 0,
        string.data(), static_cast<int>(string.size()),
        result.data(), static_cast<int>(result.size()),
        nullptr, nullptr
    );
    if (actualLength != expectedLength)
    {
        throw std::runtime_error(
            "WideCharToMultiByte failed converting string, error = " + std::to_string(GetLastError()));
    }

    return result;
}

} // namespace platform::text

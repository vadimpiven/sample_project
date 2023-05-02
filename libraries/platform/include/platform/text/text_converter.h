#pragma once

#include <string>

namespace platform::text {

struct TextConverter
{
    [[nodiscard]] static std::wstring Utf8ToUtf16(const std::string & string);
    [[nodiscard]] static std::string Utf16ToUtf8(const std::wstring & string);
};

} // namespace platform::text

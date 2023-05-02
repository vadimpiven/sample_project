#include <platform/text/text_converter.h>

#include <stdexcept>

namespace platform::text {

[[nodiscard]] std::wstring TextConverter::Utf8ToUtf16(const std::string & string)
{
    throw std::logic_error("Not implemented");
}

[[nodiscard]] std::string TextConverter::Utf16ToUtf8(const std::wstring & string)
{
    throw std::logic_error("Not implemented");
}

} // namespace platform::text

#ifndef RENDER_ENGINE_STRING_ENCODING_UTIL_H
#define RENDER_ENGINE_STRING_ENCODING_UTIL_H

#include <string>

namespace StringEncodingUtil
{
    std::u32string Utf16ToUtf32(const char16_t* text, size_t length);
    std::u16string Utf32ToUtf16(const char32_t* text, size_t length);
    std::string Utf32ToUtf8(const char32_t* text, size_t length);
    std::string Utf16ToUtf8(const char16_t* text, size_t length);
    std::string WStringToString(const std::wstring& wString);
    std::wstring ToLower(const std::wstring& wString);
    std::string ToLower(const std::string& string);
}

#endif //RENDER_ENGINE_STRING_ENCODING_UTIL_H

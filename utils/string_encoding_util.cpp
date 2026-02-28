#include "string_encoding_util.h"

std::u32string StringEncodingUtil::Utf16ToUtf32(const char16_t* text, size_t length)
{
    std::u32string out;
    out.reserve(length);

    for (int i = 0; i < length; ++i)
    {
        uint32_t c = text[i];

        if (c >= 0xD800 && c <= 0xDBFF)
        {
            uint32_t high = c - 0xD800;
            uint32_t low  = text[++i] - 0xDC00;
            c = 0x10000 + ((high << 10) | low);
        }

        out.push_back(c);
    }

    return out;
}

std::u16string StringEncodingUtil::Utf32ToUtf16(const char32_t* u32, size_t length)
{
    std::u16string out;
    out.reserve(length);

    for (int i = 0; i < length; ++i)
    {
        char32_t cp = u32[i];
        if (cp <= 0xFFFF)
            out.push_back(static_cast<char16_t>(cp));
        else
        {
            cp -= 0x10000;
            out.push_back(0xD800 + (cp >> 10));
            out.push_back(0xDC00 + (cp & 0x3FF));
        }
    }

    return out;
}

std::string StringEncodingUtil::Utf32ToUtf8(const char32_t* text, size_t length)
{
    std::string out;

    for (size_t i = 0; i < length; ++i)
    {
        char32_t cp = text[i];
        if (cp <= 0x7F)
            out.push_back(static_cast<char>(cp));
        else if (cp <= 0x7FF)
        {
            out.push_back(static_cast<char>(0xC0 | (cp >> 6)));
            out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        }
        else if (cp <= 0xFFFF)
        {
            out.push_back(static_cast<char>(0xE0 | (cp >> 12)));
            out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        }
        else
        {
            out.push_back(static_cast<char>(0xF0 | (cp >> 18)));
            out.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        }
    }

    return out;
}

std::string StringEncodingUtil::Utf16ToUtf8(const char16_t* text, size_t length)
{
    std::string out;

    for (size_t i = 0; i < length; ++i)
    {
        uint32_t cp = text[i];

        if (cp >= 0xD800 && cp <= 0xDBFF)
        {
            uint32_t high = cp - 0xD800;
            uint32_t low  = text[++i] - 0xDC00;
            cp = 0x10000 + ((high << 10) | low);
        }

        if (cp <= 0x7F)
            out.push_back(cp);
        else if (cp <= 0x7FF)
        {
            out.push_back(0xC0 | (cp >> 6));
            out.push_back(0x80 | (cp & 0x3F));
        }
        else if (cp <= 0xFFFF)
        {
            out.push_back(0xE0 | (cp >> 12));
            out.push_back(0x80 | ((cp >> 6) & 0x3F));
            out.push_back(0x80 | (cp & 0x3F));
        }
        else
        {
            out.push_back(0xF0 | (cp >> 18));
            out.push_back(0x80 | ((cp >> 12) & 0x3F));
            out.push_back(0x80 | ((cp >> 6) & 0x3F));
            out.push_back(0x80 | (cp & 0x3F));
        }
    }

    return out;
}

std::string StringEncodingUtil::WStringToString(const std::wstring& wString)
{
    if constexpr (sizeof(wchar_t) == 4)
        return Utf32ToUtf8(reinterpret_cast<const char32_t*>(wString.c_str()), wString.size());
    return Utf16ToUtf8(reinterpret_cast<const char16_t*>(wString.c_str()), wString.size());
}

std::string StringEncodingUtil::ToLower(const std::string& string)
{
    std::string result(string);

    for (char& c : result)
        c = std::tolower(c);

    return result;
}
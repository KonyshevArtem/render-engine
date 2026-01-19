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

#pragma once
#include <string>

namespace ViewUtil {

// UTF-8 문자열의 콘솔 표시 너비 계산 (ASCII=1칸, 한글/CJK 3·4바이트=2칸)
inline int DisplayWidth(const std::string& s) {
    int width = 0;
    size_t i = 0;
    while (i < s.size()) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        if      (c < 0x80) { ++width;    ++i; }   // ASCII
        else if (c < 0xE0) { ++width;    i += 2; } // 2-byte (Latin 등)
        else if (c < 0xF0) { width += 2; i += 3; } // 3-byte (한글/CJK)
        else                { width += 2; i += 4; } // 4-byte
    }
    return width;
}

// 표시 너비 기준 오른쪽 공백 패딩 (left-align)
inline std::string PadRight(const std::string& s, int targetWidth) {
    int pad = targetWidth - DisplayWidth(s);
    return pad > 0 ? s + std::string(pad, ' ') : s;
}

// 표시 너비 기준 왼쪽 공백 패딩 (right-align)
inline std::string PadLeft(const std::string& s, int targetWidth) {
    int pad = targetWidth - DisplayWidth(s);
    return pad > 0 ? std::string(pad, ' ') + s : s;
}

} // namespace ViewUtil

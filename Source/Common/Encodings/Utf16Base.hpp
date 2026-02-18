#pragma once

#include "Common/Encodings/Base.hpp"

// can accept wchar_t or char16_t
template <typename C>
struct Utf16EncodingBase {
  static constexpr size_t CharLength(const C* str) noexcept {
    uint16_t c = *str;
    return (c >> 10) == 0b110110 ? 2 : 1;
  }

  static constexpr size_t UnicodeLength(const C* str) noexcept {
    size_t length = 0;
    for (; *str; str += CharLength(str)) ++length;
    return length;
  }

  static constexpr char32_t Next(const C*& str) noexcept {
    auto l = CharLength(str);
    if (l == 1)
      return *str++;
    uint16_t high = *str++ - 0xD800;
    uint16_t low = *str++ - 0xDC00;
    return (high << 10) + low + 0x10000;
  }

  template <typename T>
  static constexpr std::basic_string<C> Encode(const T* src) noexcept {
    using From = Encoding<T>;

    std::basic_string<C> result;
    char32_t codep;

    result.reserve(64);

    while ((codep = From::Next(src))) {
      if (codep > 0xffff) {
        result.push_back(0xD7C0 + (codep >> 10));
        result.push_back(0xDC00 + (codep & 0x3FF));
      }
      else {
        result.push_back(static_cast<C>(codep));
      }
    }

    return result;
  }
};

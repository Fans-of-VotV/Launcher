#pragma once

#include "Common/Encodings/Base.hpp"

template <>
struct Encoding<char8_t> {
  static constexpr uint32_t ACCEPT = 0;
  static constexpr uint32_t REJECT = 1;

  static constexpr uint8_t utf8d[] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 00..1f
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 20..3f
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 40..5f
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 60..7f
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, // 80..9f
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, // a0..bf
    8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // c0..df
    0xa,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x4,0x3,0x3, // e0..ef
    0xb,0x6,0x6,0x6,0x5,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8, // f0..ff
    0x0,0x1,0x2,0x3,0x5,0x8,0x7,0x1,0x1,0x1,0x4,0x6,0x1,0x1,0x1,0x1, // s0..s0
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1, // s1..s2
    1,2,1,1,1,1,1,2,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1, // s3..s4
    1,2,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,3,1,1,1,1,1,1, // s5..s6
    1,3,1,1,1,1,1,3,1,3,1,1,1,1,1,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // s7..s8
  };

  // codep can be nullptr for only validating
  static constexpr uint32_t DecodeStep(uint32_t* state, char32_t* codep, uint32_t byte) noexcept {
    uint32_t type = utf8d[byte];
    uint32_t st = *state;

    if (codep != nullptr) {
      *codep = (st != 0)
               ? (byte & 0x3fu) | (*codep << 6)
               : (0xff >> type) & (byte);
    }

    return *state = utf8d[256 + st*16 + type];
  }

  static constexpr size_t UnicodeLength(const char8_t* str) noexcept {
    uint32_t state = 0;
    size_t count = 0;

    for (; *str; ++str)
      if (!DecodeStep(&state, nullptr, *str))
        count += 1;

    return state != ACCEPT ? count : -1;
  }

  static constexpr bool Validate(const char8_t* str) noexcept {
    uint32_t state = 0;

    for (; *str; ++str)
      if (DecodeStep(&state, nullptr, *str) == REJECT)
        return false;

    return true;
  }

  static constexpr char32_t Next(const char8_t*& str) noexcept {
    uint32_t state = 0;
    char32_t codep = 0;

    for (; *str; ++str) {
      switch (DecodeStep(&state, &codep, *str)) {
        case ACCEPT:
          ++str; return codep;
        case REJECT:
          if (!*str) return 0;
          ++str; return 0xFFFD;
      }
    }

    return 0;
  }

  template <typename T>
  static constexpr std::u8string Encode(const T* src) noexcept {
    using From = Encoding<T>;

    std::u8string result;
    char32_t codep;

    result.reserve(64);

    while ((codep = From::Next(src))) {
      if (codep <= 0x7F) {
        result.push_back(static_cast<char8_t>(codep));
      }
      else if (codep <= 0x7FF) {
        result.push_back((codep >> 6) | 0xC0);
        result.push_back((codep & 0x3F) | 0x80);
      }
      else if (codep <= 0xFFFF) {
        result.push_back((codep >> 12) | 0xE0);
        result.push_back(((codep >> 6) & 0x3F) | 0x80);
        result.push_back((codep & 0x3F) | 0x80);
      }
      else {
        result.push_back((codep >> 18) | 0xF0);
        result.push_back(((codep >> 12) & 0x3F) | 0x80);
        result.push_back(((codep >> 6) & 0x3F) | 0x80);
        result.push_back((codep & 0x3F) | 0x80);
      }
    }

    return result;
  }
};

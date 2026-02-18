#pragma once

#include "Common/Encodings/Base.hpp"

template <>
struct Encoding<char32_t> {
  static constexpr size_t UnicodeLength(const char32_t* str) noexcept {
    size_t result = 0;
    for (; *str; ++str)
      ++result;
    return result;
  }

  static constexpr bool Validate(const char32_t*) noexcept {
    return true;
  }

  static constexpr char32_t Next(const char32_t*& str) noexcept {
    return *str++;
  }

  template <typename T>
  static constexpr std::u32string Encode(const T* src) noexcept {
    using From = Encoding<T>;

    std::u32string result;
    char32_t codep;

    result.reserve(64);

    while ((codep = From::Next(src)))
      result.push_back(codep);

    return result;
  }
};

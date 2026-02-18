#pragma once

#include <string>

template <typename T>
struct Encoding {
  // Return string's length in Unicode chars (UTF-32)
  static constexpr size_t UnicodeLength(const T* str) noexcept {
    static_assert(std::_Always_false<T>, "Invalid T in Encoding<T>");
    (void)str;
    return 0;
  }

  // Checks if given string is valid for this encoding
  static constexpr bool Validate(const T* str) noexcept {
    static_assert(std::_Always_false<T>, "Invalid T in Encoding<T>");
    (void)str;
    return false;
  }

  // Decode char at "str" and return Unicode (UTF-32) char
  // Moves "str" to the next char
  static constexpr char32_t Next(const T*& str) noexcept {
    static_assert(std::_Always_false<T>, "Invalid T in Encoding<T>");
    (void)str;
    return 0;
  }

  // Encode string from Encoding<Y> to this encoding
  // Will use Encoding<Y>::Next()
  template <typename Y>
  static constexpr std::basic_string<Y> Encode(const Y* src) noexcept {
    static_assert(std::_Always_false<T>, "Invalid T in Encoding<T>");
    (void)src;
    return {};
  }
};

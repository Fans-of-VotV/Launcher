#pragma once

#include <array>

namespace LogUtils {

constexpr size_t MAX_LENGTH = 64;

// case-insensitive comparison with '/' == '\\'
consteval bool IsPathCharsSame(char a, char b) {
  if (a >= 'A' && a <= 'Z')
    a |= 0x20;
  if (b >= 'A' && b <= 'Z')
    b |= 0x20;
  return a == b || (a == '\\' && b == '/') || (a == '/' && b == '\\');
}

consteval std::array<char, MAX_LENGTH> MakeRelPath(const char* filePath) {
  constexpr char Root[] = _LOGGING_BASE_PATH;
  constexpr auto RootLen = sizeof(Root) - 1;
  static_assert(Root[RootLen - 1] == '/');

  bool valid = true;
  size_t i = 0;
  for (; i != RootLen; ++i) {
    if (!filePath[i] || !IsPathCharsSame(filePath[i], Root[i])) {
      valid = false;
      break;
    }
  }

  std::array<char, MAX_LENGTH> result = {};

  if (!valid)
    i = 0;

  size_t j = 0;
  for (; j < result.size() - 1 && filePath[i]; ++i, ++j) {
    char c = filePath[i];
    if (c == '/')
      c = '\\';
    result[j] = c;
  }
  result[j] = 0;

  return result;
}

} // namespace LogUtils

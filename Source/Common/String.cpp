#include "String.hpp"

bool String::contains(String const& substring) const {
  if (substring.empty())
    return true;

  auto length = substring.m_length;

  if (length > m_length)
    return false;

  for (uint i = 0; i <= m_length - length; ++i) {
    bool same = true;
    for (uint j = 0; j < length; ++j) {
      if (m_buffer[i + j] != substring[j]) {
        same = false;
        break;
      }
    }

    if (same)
      return true;
  }

  return false;
}

uint64_t String::_parseBigInt(bool* outValid) const noexcept {
  uint64_t result = 0;
  size_t start = 0;
  bool negative = false;

  *outValid = false;

  if (empty())
    return 0;

  if (m_buffer[0] == '-') {
    negative = true;
    start = 1;
  }
  else if (m_buffer[0] == '+') {
    start = 1;
  }

  for (size_t i = start; i < m_length; ++i) {
    if (m_buffer[i] < '0' || m_buffer[i] > '9')
      return 0;

    result = (result * 10) + (m_buffer[i] - '0');
  }

  *outValid = true;
  return negative ? -result : result;
}

void String::_decodeFromANSI(const char* string, size_t lengthInBytes, int codePage) {
  if (codePage == CP_ACP)
    codePage = GetACP();

  int requiredSize =
    MultiByteToWideChar(codePage, 0, string, static_cast<int>(lengthInBytes), nullptr, 0);

  auto buffer = new wchar_t[requiredSize];
  int written =
    MultiByteToWideChar(codePage, 0, string, static_cast<int>(lengthInBytes), buffer, requiredSize);

  _decodeFrom<char16_t>((const char16_t*)buffer, written);
  delete[] buffer;
}

std::string String::_encodeToANSI(int codePage) const {
  if (codePage == CP_ACP)
    codePage = GetACP();

  auto utf16 = _encodeTo<char16_t>();

  int requiredSize = WideCharToMultiByte(
    codePage,
    0,
    (const wchar_t*)utf16.data(),
    static_cast<int>(utf16.size()),
    nullptr,
    0,
    nullptr,
    nullptr
  );

  std::string result;
  result.resize(requiredSize);

  int written = WideCharToMultiByte(
    codePage,
    0,
    (const wchar_t*)utf16.data(),
    static_cast<int>(utf16.size()),
    result.data(),
    requiredSize,
    nullptr,
    nullptr
  );

  result.resize(written);
  return result;
}

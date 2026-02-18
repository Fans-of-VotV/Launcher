#pragma once

#include "Common/Encodings/Utf16.hpp"
#include "Common/Encodings/Utf32.hpp"
#include "Common/Encodings/Utf8.hpp"
#include "Common/Encodings/Wide.hpp"
#include <fmt/core.h>
#include <optional>
#include <string>

template <typename T>
concept EncodedChar = is_any_of_v<T, char8_t, char16_t, char32_t, wchar_t>;

template <typename T>
static constexpr size_t stringLength(const T* string) {
  size_t result = 0;
  for (; *string; ++string)
    ++result;
  return result;
}

/// UTF-32 String with encodings
class String {
public:
  using ValueType = char32_t;

private:
  char32_t* m_buffer = nullptr;
  size_t m_length = 0;    // IN DWORDS
  size_t m_allocated = 0; // IN DWORDS

public:
  /// Default constructor

  constexpr String() = default;

  /// Main constructors

  template <EncodedChar T>
  inline String(const T* rawString, size_t lengthInChars) : String() {
    _decodeFrom(rawString, lengthInChars);
  }

  template <EncodedChar T>
  inline implicit String(const T* rawString) : String(rawString, stringLength(rawString)) {}

  template <EncodedChar T>
  inline implicit String(std::basic_string<T> const& string)
    : String(string.data(), string.length()) {}

  template <EncodedChar T>
  inline implicit String(std::basic_string_view<T> const& string)
    : String(string.data(), string.length()) {}

  /// Moving

  constexpr String(String&& other) noexcept {
    _steal(other);
  }

  inline String& operator=(String&& other) noexcept {
    _reset();
    _steal(other);
    return *this;
  }

  /// Copying

  inline String(String const& other) noexcept : String() {
    if (other.empty())
      return;

    *this = other;
  }

  inline String& operator=(String const& other) noexcept {
    clear();

    _wantWrite(other.m_length);
    memcpy(m_buffer, other.m_buffer, (m_length = other.m_length) * sizeof(char32_t));
    m_buffer[m_length] = 0;

    return *this;
  }

  /// Destructor

  inline ~String() {
    _reset();
  }

private:
  struct ansi_init_tag_t {
    explicit constexpr ansi_init_tag_t() = default;
  };
  static constexpr auto ansi_init_tag = ansi_init_tag_t();

  /// ANSI constructor
  inline String(const char* string, size_t lengthInBytes, uint32_t codePage, const ansi_init_tag_t)
    : String() {
    _decodeFromANSI(string, lengthInBytes, codePage);
  }

public:
  /// ANSI/UTF8 Factories

  static inline String FromANSI(const char* string, size_t lengthInBytes, uint32_t codePage = 0) {
    return String(string, lengthInBytes, codePage, ansi_init_tag);
  }

  static inline String FromANSI(std::string_view const& string, uint32_t codePage = 0) {
    return String(string.data(), string.length(), codePage, ansi_init_tag);
  }

  static inline String FromUTF8(const char* string, size_t lengthInBytes) {
    return String((const char8_t*)string, lengthInBytes);
  }

  static inline String FromUTF8(std::string_view const& string) {
    return String((const char8_t*)string.data(), string.length());
  }

  // Guess encoding at runtime (UTF8 or ANSI with default code page)
  static inline String FromAuto(const char* string) {
    return FromAuto(string, stringLength(string));
  }

  static inline String FromAuto(std::string_view const& string) {
    return FromAuto(string.data(), string.length());
  }

  static inline String FromAuto(const char* string, size_t lengthInBytes) {
    auto u8string = (const char8_t*)string;

    if (Encoding<char8_t>::Validate(u8string))
      return String(u8string, lengthInBytes);

    return String(string, lengthInBytes, 0, ansi_init_tag);
  }

public:
  /// Common methods

  constexpr char32_t* begin() noexcept {
    return m_buffer;
  }

  constexpr char32_t const* begin() const noexcept {
    return m_buffer;
  }

  constexpr char32_t* end() noexcept {
    return m_buffer + m_length;
  }

  constexpr char32_t const* end() const noexcept {
    return m_buffer + m_length;
  }

  constexpr bool empty() const {
    return !m_length;
  }

  constexpr size_t length() const {
    return m_length;
  }

  constexpr size_t allocated() const {
    return m_allocated;
  }

  constexpr char32_t* data() {
    return m_buffer;
  }

  constexpr const char32_t* data() const {
    return m_buffer;
  }

  constexpr void clear() {
    if (m_buffer) {
      m_buffer[0] = 0;
      m_length = 0;
    }
  }

  bool contains(String const& substring) const;

  template <typename T>
  inline std::optional<T> parseInt() const noexcept {
    using parsed_t = std::conditional_t<std::is_unsigned_v<T>, uint64_t, int64_t>;

    bool valid;
    parsed_t parsed = std::bit_cast<parsed_t>(_parseBigInt(&valid));

    if (parsed < std::numeric_limits<T>::min() || parsed > std::numeric_limits<T>::max())
      return std::nullopt;

    if (valid)
      return (T)parsed;

    return std::nullopt;
  }

  /// Comparison

  constexpr int operator<=>(String const& other) const noexcept {
    if (m_buffer == other.m_buffer)
      return 0;

    if (empty())
      return other.empty() ? 0 : -1;
    if (other.empty())
      return 1;

    size_t minLen = m_length < other.m_length ? m_length : other.m_length;

    int result = 0;
    for (uint i = 0; i < minLen; ++i) {
      auto a = m_buffer[i], b = other.m_buffer[i];
      if (a != b) {
        result = a < b ? -1 : +1;
        break;
      }
    }

    if (result != 0)
      return result;

    if (m_length < other.m_length)
      return -1;
    if (m_length > other.m_length)
      return 1;

    return 0;
  }

  constexpr bool operator==(String const& other) const noexcept {
    return (*this <=> other) == 0;
  }
  constexpr bool operator!=(String const& other) const noexcept {
    return (*this <=> other) != 0;
  }
  constexpr bool operator>(String const& other) const noexcept {
    return (*this <=> other) == 1;
  }
  constexpr bool operator<(String const& other) const noexcept {
    return (*this <=> other) == -1;
  }
  constexpr bool operator>=(String const& other) const noexcept {
    return (*this <=> other) != -1;
  }
  constexpr bool operator<=(String const& other) const noexcept {
    return (*this <=> other) != 1;
  }

  constexpr char32_t operator[](size_t index) const noexcept {
    return m_buffer[index];
  }
  constexpr char32_t& operator[](size_t index) noexcept {
    return m_buffer[index];
  }

public:
  /// Encoding

  template <EncodedChar T>
  inline std::basic_string<T> toSTL() const {
    if (empty())
      return {};

    return _encodeTo<T>();
  }

  inline std::string toUTF8() const {
    if (empty())
      return {};

    // some tricks

    std::u8string u8str = toSTL<char8_t>();
    std::string result = std::move(*(std::string*)&u8str);

    return result;
  }

  inline std::string toANSI(int codePage = 0) const {
    if (empty())
      return {};

    return _encodeToANSI(codePage);
  }

private:
  /// Private shit

  uint64_t _parseBigInt(bool* outValid) const noexcept;
  void _decodeFromANSI(const char* string, size_t lengthInBytes, int codePage);

  template <typename T>
  inline void _decodeFrom(const T* string, size_t lengthInChars) {
    if constexpr (std::is_same_v<T, char32_t>) {
      m_length = 0;
      _wantWrite(lengthInChars);
      memcpy(m_buffer, string, lengthInChars * sizeof(char32_t));
      m_buffer[m_length = lengthInChars] = 0;
    }
    else {
      using From = Encoding<T>;

      m_length = 0;
      _wantWrite(lengthInChars);

      char32_t codep;
      auto p = string;

      while (p - string < static_cast<ptrdiff_t>(lengthInChars) && ((codep = From::Next(p)))) {
        // Just to be sure!
        _wantWrite(1);

        // ReSharper disable once CppDFANotInitializedField
        m_buffer[m_length++] = codep;
      }

      // ReSharper disable once CppDFANotInitializedField
      m_buffer[m_length] = 0;
    }
  }

  std::string _encodeToANSI(int codePage) const;

  template <typename T>
  inline std::basic_string<T> _encodeTo() const {
    return Encoding<T>::template Encode<char32_t>(m_buffer);
  }

  inline void _reset() {
    if (m_buffer)
      delete[] m_buffer;

    m_buffer = nullptr;
    m_length = m_allocated = 0;
  }

  constexpr void _steal(String& other) {
    m_buffer = other.m_buffer;
    m_length = other.m_length;
    m_allocated = other.m_allocated;
    other.m_buffer = nullptr;
    other.m_length = other.m_allocated = 0;
  }

  inline void _wantWrite(size_t charsCount) {
    auto required = m_length + charsCount + 1;

    if (m_allocated < required) {
      auto newSize = m_allocated * 2;
      if (newSize < required)
        newSize = required;

      _reallocate(newSize);
    }
  }

  inline void _reallocate(size_t newSize) {
    if (newSize < m_length + 1)
      newSize = m_length + 1;

    // ReSharper disable once CppDFANotInitializedField
    auto prevData = m_buffer;
    m_buffer = new char32_t[newSize];
    memset(m_buffer, 0, newSize);
    m_allocated = newSize;

    if (prevData) {
      memcpy(m_buffer, prevData, m_length * sizeof(char32_t));
      delete[] prevData;
    }
  }
};

template <>
struct fmt::formatter<String> : formatter<std::string_view> {
  auto format(String const& string, format_context& ctx) const {
    auto encoded = string.toUTF8();
    return formatter<std::string_view>::format(encoded, ctx);
  }
};

template <>
struct std::hash<String> {
  constexpr size_t operator()(String const& string) const noexcept {
    size_t result = 2166136261U;
    auto str = string.data();
    auto len = string.length();
    for (size_t i = 0; i < len; ++i) {
      result ^= (size_t)str[i];
      result *= 16777619U;
    }
    return result;
  }
};

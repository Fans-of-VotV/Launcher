#pragma once

#include "Common/Box.hpp"
#include "Common/Logging/utils.hpp"
#include <fmt/os.h>
#include <string_view>

// Currently doesn't do anything, but in future we may ignore some of them
enum class LogLevel : uint8_t {
  Verbose,
  Debug,
  Info,
  Warning,
  Error,
  Fatal
};

class Logger {
  STATIC_CLASS(Logger);

public:
  static void Initialize(const char* logFilePath);
  static void Terminate();

  static void Print(LogLevel level, std::string_view text, char const* file, int line);

private:
  static inline Box<fmt::ostream> m_file;
};

namespace Log {

#define MAKE_LEVEL_LOG(Level)                                                                      \
  template <typename... ArgsT>                                                                     \
  struct Level {                                                                                   \
    __forceinline Level(                                                                           \
      fmt::format_string<ArgsT...> fmt,                                                            \
      ArgsT&&... args,                                                                             \
      std::array<char, LogUtils::MAX_LENGTH> const& file =                                         \
        LogUtils::MakeRelPath(__builtin_FILE()),                                                   \
      uint line = __builtin_LINE()                                                                 \
    ) {                                                                                            \
      Logger::Print(                                                                               \
        LogLevel::Level, fmt::format(fmt, std::forward<ArgsT>(args)...), file.data(), line         \
      );                                                                                           \
    }                                                                                              \
  };                                                                                               \
  template <typename... ArgsT>                                                                     \
  Level(auto fmt, ArgsT&&...)->Level<ArgsT...>

MAKE_LEVEL_LOG(Verbose);
MAKE_LEVEL_LOG(Debug);
MAKE_LEVEL_LOG(Info);
MAKE_LEVEL_LOG(Warning);
MAKE_LEVEL_LOG(Error);
MAKE_LEVEL_LOG(Fatal);

#undef MAKE_LEVEL_LOG

} // namespace Log

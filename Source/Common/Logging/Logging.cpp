#include "Logging.hpp"

#include <chrono>
#include <fmt/chrono.h>

void Logger::Initialize(const char* logFilePath) {
  m_file.Initialize(fmt::output_file(logFilePath));
}

void Logger::Terminate() {
  m_file.Destroy();
}

void Logger::Print(LogLevel level, std::string_view text, char const* file, int line) {
  using namespace std::chrono;

  // Ignored for now
  (void)level;
  (void)file;
  (void)line;

  auto sendTime =
    floor<microseconds>(zoned_time(current_zone(), system_clock::now()).get_local_time());
  auto prefix = fmt::format("[{:%H:%M:%S}]", sendTime);

  fmt::print("\x1B[90m{}\x1B[39m {}\n", prefix, text);
  m_file->print("{} {}\n", prefix, text);
  m_file->flush();
}

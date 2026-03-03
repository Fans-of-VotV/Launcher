#include <rh/log.hpp>

extern "C" void _AssertationFailureHandler(const char* expr, const wchar_t* file, uint line) {
  Log::Fatal("Assertation failure! Expression: {}", expr);
  Log::Fatal("In file {} at line {}", String(file), line);
}

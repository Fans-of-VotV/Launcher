#include "Asserts.hpp"

#include "Common/Logging/Logging.hpp"

void MyAssertFailure(const char* expr, const char* file, int line) {
  Log::Error("Assertion failure: {}", expr);
}

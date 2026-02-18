#pragma once

#include "Common/Logging/Logging.hpp"
#include "Common/String.hpp"

String Win32ErrorToString(DWORD errorCode);

inline String GetLastErrorString() noexcept {
  return Win32ErrorToString(GetLastError());
}

forceinline bool _ReportHRESULT(
  HRESULT hr,
  std::array<char, LogUtils::MAX_LENGTH> const& file = LogUtils::MakeRelPath(__builtin_FILE()),
  uint line = __builtin_LINE()
) noexcept {
  if (SUCCEEDED(hr))
    return false;

  Logger::Print(
    LogLevel::Error,
    fmt::format("HRESULT Failed (0x{:08X}): {}", (uint32_t)hr, Win32ErrorToString(hr)).c_str(),
    file.data(),
    line
  );

  return true;
}

// Works like FAILED() but also logs failed HRESULTs
#define REPORT_HRESULT(...) _ReportHRESULT((__VA_ARGS__))

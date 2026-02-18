#include "Win32.hpp"

String Win32ErrorToString(DWORD errorCode) {
  LPWSTR messageBuffer = nullptr;
  FormatMessageW(
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
    nullptr,
    errorCode,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    (LPWSTR)&messageBuffer,
    0,
    nullptr
  );

  if (!messageBuffer)
    return L"FormatMessageW() resulted with nullptr";

  std::wstring info = messageBuffer;
  LocalFree(messageBuffer);

  // Stupid fucking nasty microsoft for some stupid dumb reason puts new line at the end
  // OBLITERATE IT!!!!!!!!!!
  ptrdiff_t i = info.length() - 1;
  while (i > 0 && isspace(info[i]))
    --i;
  info.resize(i + 1);

  return info;
}
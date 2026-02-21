#include "WebViewProvider.hpp"

#include <string>

#define WEBVIEW_API_VERSION         1150
#define WEBVIEW_TARGET_RELEASE_GUID L"{F3017226-FE2A-4295-8BDF-00C3A9A7E4C5}"

bool WebViewProvider::SearchForImpl() {
  if (SearchForInstalledClient(true))
    return true;
  return SearchForInstalledClient(false);
}

bool WebViewProvider::SearchForInstalledClient(bool system) {
  auto rootKey = system ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER;
  auto subKey = L"SOFTWARE\\Microsoft\\EdgeUpdate\\ClientState\\" WEBVIEW_TARGET_RELEASE_GUID;

  HKEY regKey = nullhandle;
  LSTATUS status = RegOpenKeyExW(rootKey, subKey, 0, KEY_READ | KEY_WOW64_32KEY, &regKey);
  if (status != ERROR_SUCCESS)
    return false;

  DWORD length = 0;
  status = RegQueryValueExW(regKey, L"EBWebView", nullptr, nullptr, nullptr, &length);
  if (status != ERROR_SUCCESS)
    return false;

  auto buffer = new wchar_t[length / sizeof(wchar_t)];
  RegQueryValueExW(
    regKey, L"EBWebView", nullptr, nullptr, reinterpret_cast<LPBYTE>(&buffer[0]), &length
  );

  RegCloseKey(regKey);

  int i = (length / sizeof(wchar_t)) - 1;
  for (; i >= 0; --i) {
    if (buffer[i] == L'\\')
      break;
  }

  // auto versionString = buffer + i + 1;
  // TODO check API version, but do we give a fuck?

  std::wstring dllPath = buffer;
  delete[] buffer;

  if (dllPath.empty())
    return false;

  if (dllPath.back() != L'\\' && dllPath.back() != L'/')
    dllPath += L'\\';

  dllPath += L"EBWebView\\"
#if defined(_M_X64) || defined(__x86_64__)
             L"x64"
#elif defined(_M_IX86) || defined(__i386__)
             L"x86"
#elif defined(_M_ARM64) || defined(__aarch64__)
             L"arm64"
#else
#error WebView2 integration for this platform is not yet supported.
#endif
             L"\\EmbeddedBrowserWebView.dll";

  DoLoadDLL(dllPath);
  return true;
}

void WebViewProvider::DoLoadDLL(std::wstring_view dllPath) {
  Log::Verbose("Loading WebView2 implementation from dll: {:?}", String(dllPath));

  // FIXME stupid c++ standard drafters issue: string_view can be not null-terminated
  //  let's pray for the best
  HMODULE lib = LoadLibraryW(dllPath.data());

  CreateWebViewEnvironmentWithOptionsInternal =
    reinterpret_cast<CreateWebViewEnvironmentWithOptionsInternal_t>(
      GetProcAddress(lib, "CreateWebViewEnvironmentWithOptionsInternal")
    );
}

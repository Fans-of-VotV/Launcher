#pragma once

#include <WebView2.h>
#include <string_view>

enum class webview2_runtime_type {
  installed = 0,
  embedded = 1
};

using CreateWebViewEnvironmentWithOptionsInternal_t = HRESULT(STDMETHODCALLTYPE*)(
  bool,
  webview2_runtime_type,
  PCWSTR,
  IUnknown*,
  ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler*
);

class WebViewProvider {
  STATIC_CLASS(WebViewProvider);

public:
  static inline CreateWebViewEnvironmentWithOptionsInternal_t
    CreateWebViewEnvironmentWithOptionsInternal = nullptr;

public:
  static bool SearchForImpl();

private:
  static bool SearchForInstalledClient(bool system);
  static void DoLoadDLL(std::wstring_view dllPath);
};

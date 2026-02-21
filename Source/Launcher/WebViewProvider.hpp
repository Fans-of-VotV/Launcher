#pragma once

#include "Common/Logging/Asserts.hpp"
#include <WebView2.h>
#include <string_view>

using CreateWebViewEnvironmentWithOptionsInternal_t = HRESULT(STDMETHODCALLTYPE*)(
  bool,
  int,
  PCWSTR,
  ICoreWebView2EnvironmentOptions*,
  ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler*
);

class WebViewProvider {
  STATIC_CLASS(WebViewProvider);

public:
  static bool SearchForImpl();

  static inline HRESULT CreateEnvironment(
    PCWSTR userDataDir,
    ICoreWebView2EnvironmentOptions* options,
    ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler* handler
  ) {
    ASSERT(CreateWebViewEnvironmentWithOptionsInternal != nullptr);
    return CreateWebViewEnvironmentWithOptionsInternal(true, 0, userDataDir, options, handler);
  }

private:
  static bool SearchForInstalledClient(bool system);
  static void DoLoadDLL(std::wstring_view dllPath);

private:
  static inline CreateWebViewEnvironmentWithOptionsInternal_t
    CreateWebViewEnvironmentWithOptionsInternal = nullptr;
};

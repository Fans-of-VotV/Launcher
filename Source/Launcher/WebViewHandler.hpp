#pragma once

#include "Common/COM/CO.hpp"
#include <WebView2.h>

class Application;

class WebViewHandler final
  : public CommonObject<
      ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler,
      ICoreWebView2CreateCoreWebView2ControllerCompletedHandler,
      ICoreWebView2WebResourceRequestedEventHandler> {
public:
  DECLARE_COMMON_OBJECT_MAKER(WebViewHandler)

private:
  explicit WebViewHandler(Application* app);

public:
  // ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler:
  HRESULT STDMETHODCALLTYPE
  Invoke(HRESULT errorCode, ICoreWebView2Environment* createdEnvironment) override;

  // ICoreWebView2CreateCoreWebView2ControllerCompletedHandler:
  HRESULT STDMETHODCALLTYPE
  Invoke(HRESULT errorCode, ICoreWebView2Controller* createdController) override;

  // ICoreWebView2WebResourceRequestedEventHandler:
  HRESULT STDMETHODCALLTYPE
  Invoke(ICoreWebView2* sender, ICoreWebView2WebResourceRequestedEventArgs* args) override;

private:
  Application* m_app;
};

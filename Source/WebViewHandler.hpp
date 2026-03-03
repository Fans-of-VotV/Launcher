#pragma once

#include "WebView.hpp"
#include <rh/win32/com/CommonObject.hpp>

class Application;

class WebViewHandler final
  : public CommonObject<
      ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler,
      ICoreWebView2CreateCoreWebView2ControllerCompletedHandler,
      ICoreWebView2WebResourceRequestedEventHandler,
      ICoreWebView2WebMessageReceivedEventHandler> {
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

  // ICoreWebView2WebMessageReceivedEventHandler:
  HRESULT STDMETHODCALLTYPE
  Invoke(ICoreWebView2* sender, ICoreWebView2WebMessageReceivedEventArgs* args) override;

private:
  Application* m_app;
};

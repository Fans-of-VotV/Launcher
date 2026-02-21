#include "WebViewHandler.hpp"

#include "Launcher/Application.hpp"

WebViewHandler::WebViewHandler(Application* app) : m_app(app) {}

// ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler:
HRESULT WebViewHandler::Invoke(HRESULT errorCode, ICoreWebView2Environment* createdEnvironment) {
  m_app->WebViewOnEnvironmentCreated(errorCode, createdEnvironment);
  return S_OK;
}

// ICoreWebView2CreateCoreWebView2ControllerCompletedHandler:
HRESULT WebViewHandler::Invoke(HRESULT errorCode, ICoreWebView2Controller* createdController) {
  m_app->WebViewOnControllerCreated(errorCode, createdController);
  return S_OK;
}

// ICoreWebView2WebResourceRequestedEventHandler:
HRESULT WebViewHandler::Invoke(
  ICoreWebView2* sender,
  ICoreWebView2WebResourceRequestedEventArgs* args
) {
  m_app->WebViewOnWebResourceRequested(sender, args);
  return S_OK;
}

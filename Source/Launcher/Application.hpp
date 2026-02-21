#pragma once

#include "Launcher/WebViewHandler.hpp"

/**
 * Main application class
 */
class Application {
  friend WebViewHandler;

  IMMOVABLE_CLASS(Application);

public:
  Application();
  ~Application();

public:
  int Start();

private:
  void WebViewOnEnvironmentCreated(HRESULT errorCode, ICoreWebView2Environment* env);
  void WebViewOnControllerCreated(HRESULT errorCode, ICoreWebView2Controller* ctrl);
  void WebViewOnWebResourceRequested(
    ICoreWebView2* sender,
    ICoreWebView2WebResourceRequestedEventArgs* args
  );

  void ResizeWebView();

public:
  static bool StaticInit();

private:
  static LRESULT WINAPI MainWndProc(HWND, UINT, WPARAM, LPARAM);

  static void InitConsole();
  static bool InitWebView();
  static bool SetupDpiAwareness();
  static void RegisterWindowClass();

private:
  bool m_wantExit = false;
  HWND m_window = nullhandle;
  CO<WebViewHandler> m_webViewHandler;
  CO<ICoreWebView2_5> m_webView;
  CO<ICoreWebView2Controller> m_webViewCtrl;
};

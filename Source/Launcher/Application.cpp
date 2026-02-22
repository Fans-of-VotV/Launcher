#include "Application.hpp"

#include "Common/json.hpp"
#include "Launcher/CommandLine.hpp"
#include "Launcher/WebAssets.hpp"
#include "Launcher/WebViewProvider.hpp"
#include <dwmapi.h>
#include <iostream>
#include <shlwapi.h>

// Microsoft's dumbasses issue:
//  Hosting app on non-special domains adds additional 2 seconds delay because of TLD resolving:
//  https://github.com/MicrosoftEdge/WebView2Feedback/issues/2381#issuecomment-1105457773
#define APP_DOMAIN L"app.example"

Application::Application() = default;
Application::~Application() = default;

int Application::Start() {
  REPORT_HRESULT(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED));

  m_window = CreateWindowExW(
    0,
    L"votvl-main",
    L"Voices of the Void Community Launcher",
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    1280,
    720,
    nullptr,
    nullptr,
    GetModuleHandleW(nullptr),
    this
  );

  m_webViewHandler = WebViewHandler::Create(this);
  REPORT_HRESULT(WebViewProvider::CreateEnvironment(nullptr, nullptr, m_webViewHandler.Get()));

  ShowWindow(m_window, SW_SHOW);
  UpdateWindow(m_window);
  SetFocus(m_window);

  MSG msg;
  while (!m_wantExit) {
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
    }

    SwitchToThread();
  }

  DestroyWindow(m_window);
  m_window = nullhandle;
  CoUninitialize();

  return 0;
}

void Application::OnPlayClicked() {
  Log::Debug("Play clicked");
}

void Application::WebViewOnEnvironmentCreated(HRESULT errorCode, ICoreWebView2Environment* env) {
  ASSERT(errorCode == S_OK);

  Log::Verbose("WebView2 environment created: {}", fmt::ptr(env));

  REPORT_HRESULT(env->CreateCoreWebView2Controller(m_window, m_webViewHandler.Get()));
}

void Application::WebViewOnControllerCreated(HRESULT errorCode, ICoreWebView2Controller* ctrl) {
  ASSERT(errorCode == S_OK);

  Log::Verbose("WebView2 controller created: {}", fmt::ptr(ctrl));
  m_webViewCtrl = { ctrl, AddNewReference };

  {
    CO<ICoreWebView2> webViewBase;
    REPORT_HRESULT(ctrl->get_CoreWebView2(&webViewBase));
    REPORT_HRESULT(webViewBase->QueryInterface(&m_webView));
    webViewBase.Release();
  }

  Log::Verbose("WebView2_5: {}", fmt::ptr(m_webView.Get()));

  REPORT_HRESULT(m_webView->add_WebResourceRequested(m_webViewHandler.Get(), nullptr));
  REPORT_HRESULT(m_webView->add_WebMessageReceived(m_webViewHandler.Get(), nullptr));

  REPORT_HRESULT(m_webView->AddWebResourceRequestedFilter(
    L"https://" APP_DOMAIN L"/*", COREWEBVIEW2_WEB_RESOURCE_CONTEXT_ALL
  ));

  auto targetUrl = L"https://" APP_DOMAIN L"/index.html";
  if (CommandLine::IsLocalHost)
    targetUrl = L"http://localhost:5173";

  REPORT_HRESULT(m_webView->Navigate(targetUrl));

  ResizeWebView();
  REPORT_HRESULT(ctrl->put_IsVisible(true));
}

void Application::WebViewOnWebResourceRequested(
  ICoreWebView2*,
  ICoreWebView2WebResourceRequestedEventArgs* args
) {
  CO<ICoreWebView2WebResourceRequest> request;
  REPORT_HRESULT(args->get_Request(&request));

  LPWSTR uriRaw;
  REPORT_HRESULT(request->get_Uri(&uriRaw));

  std::wstring uri = uriRaw;
  CoTaskMemFree(uriRaw);

  if (!uri.starts_with(L"https://" APP_DOMAIN L"/"))
    return;

  auto pathname = uri.substr(_countof(L"https://" APP_DOMAIN) - 1);
  auto asset = WebAssets::Get(String(pathname).toUTF8());

  Log::Debug("Fetching asset {:?} -> {}", String(pathname), fmt::ptr(asset));

  CO<ICoreWebView2WebResourceResponse> response;
  CO<ICoreWebView2Environment> env;

  REPORT_HRESULT(m_webView->get_Environment(&env));

  if (asset == nullptr) {
    REPORT_HRESULT(env->CreateWebResourceResponse(nullptr, 404, L"Not Found", nullptr, &response));
    REPORT_HRESULT(args->put_Response(response.Get()));
    return;
  }

  auto stream = SHCreateMemStream(static_cast<LPCBYTE>(asset->data), asset->dataSize);

  auto headers = fmt::format(
    "Content-Type: {}\r\n"
    "Content-Length: {}",
    asset->mimeType,
    asset->dataSize
  );

  REPORT_HRESULT(env->CreateWebResourceResponse(
    stream, 200, L"OK", String::FromUTF8(headers).toSTL<wchar_t>().c_str(), &response
  ));
  REPORT_HRESULT(args->put_Response(response.Get()));
}

void Application::WebViewOnWebMessage(
  ICoreWebView2*, ICoreWebView2WebMessageReceivedEventArgs* args
) {
  wchar_t* jsonStrRaw = nullptr;
  REPORT_HRESULT(args->get_WebMessageAsJson(&jsonStrRaw));

  auto jsonStr = String(jsonStrRaw);
  CoTaskMemFree(jsonStrRaw);

  nlohmann::json message;
  try {
    message = nlohmann::json::parse(jsonStr.toUTF8());
  }
  catch (std::exception& error) {
    Log::Error("Failed to parse JSON from web message: {}", error.what());
    return;
  }

  if (!message.contains("type"))
    return;

  std::string msgType = message["type"];

  if (msgType == "play") {
    OnPlayClicked();
  }
}

void Application::ResizeWebView() {
  if (m_webViewCtrl == nullptr)
    return;

  RECT bounds {};
  if (GetClientRect(m_window, &bounds))
    REPORT_HRESULT(m_webViewCtrl->put_Bounds(bounds));
}

bool Application::StaticInit() {
  InitConsole();
  Logger::Initialize("votv-launcher.log");
  WebAssets::Initialize();
  if (!InitWebView())
    return false;
  if (!SetupDpiAwareness())
    return false;
  RegisterWindowClass();
  return true;
}

LRESULT Application::MainWndProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam) {
  Application* parentApp = nullptr;

  if (msg == WM_NCCREATE) {
    auto createStruct = reinterpret_cast<LPCREATESTRUCTW>(lparam);

    parentApp = static_cast<Application*>(createStruct->lpCreateParams);
    SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(parentApp));

    bool nonClientDpiScalingNeeded = !AreDpiAwarenessContextsEqual(
      GetWindowDpiAwarenessContext(window), DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE
    );
    if (nonClientDpiScalingNeeded)
      EnableNonClientDpiScaling(window);

    DWORD useLightTheme = 0;
    {
      LRESULT status = RegGetValueW(
        HKEY_CURRENT_USER,
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme",
        RRF_RT_DWORD,
        nullptr,
        &useLightTheme,
        nullptr
      );

      if (status != ERROR_SUCCESS)
        useLightTheme = 0;
    }

    BOOL darkModeEnabled = !useLightTheme ? TRUE : FALSE;

    auto status = DwmSetWindowAttribute(
      window, DWMWA_USE_IMMERSIVE_DARK_MODE, &darkModeEnabled, sizeof(darkModeEnabled)
    );
    if (status != S_OK) {
      (void)DwmSetWindowAttribute(
        window,
        19 /* DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_V10_0_19041 */,
        &darkModeEnabled,
        sizeof(darkModeEnabled)
      );
    }
  }
  else {
    parentApp = reinterpret_cast<Application*>(GetWindowLongPtrW(window, GWLP_USERDATA));
  }

  if (parentApp == nullptr)
    return DefWindowProcW(window, msg, wparam, lparam);

  switch (msg) {
    case WM_CLOSE:
      parentApp->m_wantExit = true;
      break;
    case WM_SIZE:
      parentApp->ResizeWebView();
      break;

    default:
      return DefWindowProcW(window, msg, wparam, lparam);
  }

  return 0;
}

void Application::InitConsole() {
  BOOL attached = AttachConsole(ATTACH_PARENT_PROCESS);
  if (!attached)
    AllocConsole();

  DWORD consoleMode;
  HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
  GetConsoleMode(output, &consoleMode);
  SetConsoleMode(output, consoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
  SetConsoleOutputCP(CP_UTF8);

  freopen("CONOUT$", "w", stdout);
  freopen("CONOUT$", "w", stderr);
  std::ios::sync_with_stdio(true);
}

bool Application::InitWebView() {
  if (!WebViewProvider::SearchForImpl()) {
    int result = MessageBoxW(
      nullhandle,
      L"На вашем компьютере отсутствует подходящая версия WebView2\n"
      L"Нажмите \"OK\" чтобы открыть страницу загрузки WebView2",
      L"Ошибка!",
      MB_ICONERROR | MB_OKCANCEL
    );

    if (result == IDOK) {
      system("explorer \"https://developer.microsoft.com/en-us/microsoft-edge/webview2\"");
    }

    return false;
  }

  return true;
}

extern "C" NTSYSAPI NTSTATUS WINAPI RtlGetVersion(PRTL_OSVERSIONINFOW lpVersionInformation);

bool Application::SetupDpiAwareness() {
  RTL_OSVERSIONINFOW versionInfo { 0 };
  versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
  if (RtlGetVersion(&versionInfo) != 0)
    return false;

  bool isV2 = versionInfo.dwMajorVersion >= 10 && versionInfo.dwBuildNumber >= 15063;
  auto ctx =
    isV2 ? DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 : DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE;

  SetProcessDpiAwarenessContext(ctx);
  return true;
}

void Application::RegisterWindowClass() {
  WNDCLASSEXW mainClass { 0 };
  mainClass.cbSize = sizeof(mainClass);
  mainClass.hInstance = GetModuleHandleW(nullptr);
  mainClass.lpszClassName = L"votvl-main";
  mainClass.lpfnWndProc = &MainWndProc;
  RegisterClassExW(&mainClass);
}

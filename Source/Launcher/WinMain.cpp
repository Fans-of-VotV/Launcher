#include "Common/CO.hpp"
#include "Common/Logging/Logging.hpp"
#include "Common/Logging/Win32.hpp"
#include "Launcher/WebAssets.hpp"
#include "Launcher/WebViewProvider.hpp"
#include <dwmapi.h>
#include <iostream>
#include <wrl.h>

using Microsoft::WRL::Callback;

#define CLASS_NAME L"votv-community-launcher"

HWND MainWindow = nullhandle;
bool WantCloseWindow = false;

static void InitializeConsole() {
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

extern "C" NTSYSAPI NTSTATUS WINAPI RtlGetVersion(PRTL_OSVERSIONINFOW lpVersionInformation);

static void EnableDpiAwareness() {
  RTL_OSVERSIONINFOW versionInfo { 0 };
  versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
  if (RtlGetVersion(&versionInfo) != 0)
    return;

  bool isV2 = versionInfo.dwMajorVersion >= 10 && versionInfo.dwBuildNumber >= 15063;
  auto ctx =
    isV2 ? DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 : DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE;

  SetProcessDpiAwarenessContext(ctx);
}

static void EnableNonClientDpiScalingIfNeeded(HWND window) {
  auto windowAwareness = GetWindowDpiAwarenessContext(window);
  if (windowAwareness == nullptr)
    return;

  if (!AreDpiAwarenessContextsEqual(windowAwareness, DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE))
    return;

  EnableNonClientDpiScaling(window);
}

static bool IsDarkThemeEnabled() {
  DWORD useLightTheme = 0;
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

  return !useLightTheme;
}

static void ApplyWindowTheme(HWND window) {
  BOOL darkModeEnabled = IsDarkThemeEnabled() ? TRUE : FALSE;

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

static LRESULT WINAPI WndProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam) {
  switch (msg) {
    case WM_NCCREATE:
      EnableNonClientDpiScalingIfNeeded(window);
      ApplyWindowTheme(window);
      return DefWindowProcW(window, msg, wparam, lparam);
    case WM_CLOSE:
      if (window == MainWindow) {
        WantCloseWindow = true;
      }
      else {
        DestroyWindow(window);
      }
      break;

    default:
      return DefWindowProcW(window, msg, wparam, lparam);
  }

  return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int) {
  InitializeConsole();
  Logger::Initialize("votv-launcher.log");
  WebAssets::Initialize();

  REPORT_HRESULT(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED));

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

    return 1;
  }

  EnableDpiAwareness();

  WNDCLASSEXW wndClass { 0 };
  wndClass.cbSize = sizeof(wndClass);
  wndClass.hInstance = hInstance;
  wndClass.lpszClassName = CLASS_NAME;
  wndClass.lpfnWndProc = WndProc;
  RegisterClassExW(&wndClass);

  MainWindow = CreateWindowW(
    CLASS_NAME,
    L"Voices of the Void Community Launcher",
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    1280,
    720,
    nullptr,
    nullptr,
    hInstance,
    nullptr
  );

  REPORT_HRESULT(
    WebViewProvider::CreateWebViewEnvironmentWithOptionsInternal(
      true,
      webview2_runtime_type::installed,
      nullptr,
      nullptr,
      Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
        [&](HRESULT, ICoreWebView2Environment* env) -> HRESULT {
          REPORT_HRESULT(env->CreateCoreWebView2Controller(
            MainWindow,
            Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
              [&](HRESULT, ICoreWebView2Controller* ctrl) -> HRESULT {
                ctrl->AddRef();
                ICoreWebView2* webview;
                REPORT_HRESULT(ctrl->get_CoreWebView2(&webview));
                webview->AddRef();

                REPORT_HRESULT(webview->Navigate(L"https://google.com"));
                REPORT_HRESULT(ctrl->put_IsVisible(TRUE));
                RECT bounds{};
                if (GetClientRect(MainWindow, &bounds)) {
                  ctrl->put_Bounds(bounds);
                }

                return S_OK;
              }
            ).Get()
          ));

          return S_OK;
        }
      ).Get()
    )
  );

  ShowWindow(MainWindow, SW_SHOW);
  UpdateWindow(MainWindow);
  SetFocus(MainWindow);

  MSG msg;
  while (!WantCloseWindow) {
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
    }

    SwitchToThread();
  }

  DestroyWindow(MainWindow);
  UnregisterClassW(CLASS_NAME, hInstance);
  CoUninitialize();
  return 0;
}

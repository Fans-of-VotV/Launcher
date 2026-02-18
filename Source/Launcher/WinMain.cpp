#include "Common/Logging/Logging.hpp"
#include "Common/Logging/Win32.hpp"
#include "Launcher/WebAssets.hpp"
#include "Launcher/WebViewProvider.hpp"
#include <iostream>
#include <wrl.h>

using Microsoft::WRL::Callback;

static void InitializeConsole() {
  BOOL attached = AttachConsole(ATTACH_PARENT_PROCESS);

  if (!attached) {
    AllocConsole();

    DWORD consoleMode;
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleMode(output, &consoleMode);
    SetConsoleMode(output, consoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    SetConsoleOutputCP(CP_UTF8);
  }

  freopen("CONOUT$", "w", stdout);
  freopen("CONOUT$", "w", stderr);
  std::ios::sync_with_stdio(true);
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

  CoUninitialize();
  return 0;
}

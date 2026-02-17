#include <Windows.h>
#include <webview/webview.h>

int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int) {
  try {
    webview::webview w(false, nullptr);
    w.set_title("Basic Example");
    w.set_size(480, 320, WEBVIEW_HINT_NONE);
    w.set_html("Hello, world!");
    w.run();
  }
  catch (webview::exception const& exc) {
    MessageBoxA(NULL, exc.what(), "Unhandled exception", MB_ICONERROR);
    return 1;
  }

  return 0;
}

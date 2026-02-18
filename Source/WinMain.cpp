#include "WebAssets.hpp"
#include <Windows.h>

int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int) {
  WebAssets::Initialize();

  return 0;
}

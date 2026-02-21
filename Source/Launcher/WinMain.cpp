#include "Application.hpp"

int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int) {
  if (!Application::StaticInit())
    return 1;

  return std::make_unique<Application>()->Start();
}

#include "Launcher/Application.hpp"
#include "Launcher/CommandLine.hpp"

int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int) {
  CommandLine::Parse();

  if (!Application::StaticInit())
    return 1;

  return std::make_unique<Application>()->Start();
}

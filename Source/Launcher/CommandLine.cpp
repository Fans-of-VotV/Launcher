#include "CommandLine.hpp"

void CommandLine::Parse() {
  int argc = 0;
  wchar_t** argv = CommandLineToArgvW(GetCommandLineW(), &argc);

  for (int i = 1; i < argc; ++i) {
    std::wstring arg = argv[i];

    if (arg == L"--localhost") {
      IsLocalHost = true;
    }
  }

  LocalFree(argv);
}

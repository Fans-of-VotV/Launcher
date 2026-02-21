#pragma once

class CommandLine {
  STATIC_CLASS(CommandLine);

public:
  // --localhost
  // Connect to http://localhost:5173/ as frontend
  static inline bool IsLocalHost = false;

public:
  static void Parse();
};

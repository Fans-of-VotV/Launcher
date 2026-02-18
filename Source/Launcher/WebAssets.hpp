#pragma once

#include <memory>
#include <stdint.h>
#include <string>
#include <unordered_map>

struct WebAsset {
  uint id;
  std::string pathname;
  std::string mimeType;
  void const* data;
  size_t dataSize; // in bytes
};

class WebAssets {
  STATIC_CLASS(WebAssets);

public:
  static void Initialize();

  static WebAsset* Get(std::string_view pathname);

public:
  static inline std::unordered_map<std::string, std::unique_ptr<WebAsset>> m_assets;
};

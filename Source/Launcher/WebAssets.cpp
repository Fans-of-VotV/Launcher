#include "WebAssets.hpp"

void WebAssets::Initialize() {
#include "WebAssetsBundle.inc"
}

WebAsset* WebAssets::Get(std::string_view pathname) {
  auto it = m_assets.find(std::string { pathname });

  if (it == m_assets.end())
    return nullptr;

  return it->second.get();
}

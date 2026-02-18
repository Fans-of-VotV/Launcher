#include "WebAssets.hpp"

void WebAssets::Initialize() {
#define BUNDLED_ASSET(ID, PATHNAME, MIMETYPE, ...)                                                 \
  {                                                                                                \
    static uint8_t asset##ID##_data[] = { __VA_ARGS__ };                                           \
    auto pathname = std::string { PATHNAME };                                                      \
    m_assets.emplace(                                                                              \
      pathname,                                                                                    \
      std::make_unique<WebAsset>(                                                                  \
        ID, pathname, MIMETYPE, asset##ID##_data, sizeof(asset##ID##_data)                         \
      )                                                                                            \
    );                                                                                             \
  }
#include "WebAssetsBundle.inc"
#undef BUNDLED_ASSET
}

WebAsset* WebAssets::Get(std::string_view pathname) {
  auto it = m_assets.find(std::string { pathname });

  if (it == m_assets.end())
    return nullptr;

  return it->second.get();
}

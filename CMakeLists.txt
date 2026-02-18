cmake_minimum_required(VERSION 3.18)

include(CMake/WebView.cmake)

project(VotV-Community-Launcher
  VERSION 0.1.0.0
  LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake/modules")
find_webview_libs()

add_executable(main_app WIN32
  "Source/WebAssets.hpp"
  "Source/WinMain.cpp"
  "Source/Generated/WebAssets.cpp"
)

target_link_libraries(main_app PRIVATE
  MSWebView2::headers
)

target_precompile_headers(main_app PRIVATE
  "Source/StdAfx.hpp"
)

target_compile_options(main_app PRIVATE
  "/FI${CMAKE_CURRENT_SOURCE_DIR}/Source/StdAfx.hpp"
)

set_target_properties(main_app PROPERTIES
  CXX_STANDARD 20
  OUTPUT_NAME "Voices of the Void Launcher"
)

cmake_minimum_required(VERSION 3.18)

include(FetchContent)
include(CMake/WebView.cmake)

project(VotV-Community-Launcher
  VERSION 0.1.0.0
  LANGUAGES CXX
)

add_compile_definitions(
  _CRT_SECURE_NO_WARNINGS
  NOMINMAX
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  add_compile_options("/utf-8")
endif()

add_link_options("/OPT:REF,ICF")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake/modules")
find_webview_libs()

add_executable(main_app WIN32)

target_compile_definitions(main_app PRIVATE
  _LOGGING_BASE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/Source/"
)

file(GLOB_RECURSE SOURCES "Source/*.cpp" "Source/*.hpp")
target_sources(main_app PRIVATE ${SOURCES})

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        12.1.0
)

set(FMT_MASTER_PROJECT OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(fmt)

target_link_libraries(main_app PRIVATE
  MSWebView2::headers
  fmt::fmt

  ntdll.lib
  Dwmapi.lib
)

target_include_directories(main_app PRIVATE
  "Source"
  "Frontend/bundle"
)

target_precompile_headers(main_app PRIVATE "Source/Common/StdAfx.hpp")
target_compile_options(main_app PRIVATE "/FI${CMAKE_CURRENT_SOURCE_DIR}/Source/Common/StdAfx.hpp")

set_target_properties(main_app PROPERTIES
  CXX_STANDARD 20
  OUTPUT_NAME "Voices of the Void Launcher"
)

#=====================================
FetchContent_Declare(
  webview
  GIT_REPOSITORY https://github.com/webview/webview
  GIT_TAG        0.12.0
)
FetchContent_MakeAvailable(webview)
target_link_libraries(main_app PRIVATE
  webview::core
)
#=====================================

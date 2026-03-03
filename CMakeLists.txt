cmake_minimum_required(VERSION 3.18)

include(FetchContent)
include(CMake/WebView.cmake)

project(VotV-Community-Launcher
  VERSION 0.1.0.0
  LANGUAGES CXX
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  add_compile_options("/utf-8")
endif()

# Breaks cpr because of tricky shit with microsoft(obviously, how ANY shit can be without them) and
#  some other warnings
add_compile_options("-Wno-error")

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /OPT:REF,ICF")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake/modules")
find_webview_libs()

add_executable(main_app WIN32)

target_compile_definitions(main_app PRIVATE
  _LOGGING_BASE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/Source/"
)

file(GLOB_RECURSE SOURCES "Source/*.cpp" "Source/*.hpp")
target_sources(main_app PRIVATE ${SOURCES})

FetchContent_Declare(
  rhlib
  GIT_REPOSITORY https://github.com/RedHolms/rhlib.git
  GIT_TAG        v0.1.3
)

FetchContent_Declare(
  cpr
  GIT_REPOSITORY https://github.com/libcpr/cpr.git
  GIT_TAG        1.14.2
)

set(RHLIB_MODULES Core Logging)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(rhlib cpr)
add_subdirectory("ThirdParty")

target_link_libraries(main_app PRIVATE
  MSWebView2::headers
  rhlib
  json
  cpr::cpr

  ntdll.lib
  Dwmapi.lib
  Shlwapi.lib
)

target_include_directories(main_app PRIVATE
  "Source"
  "Frontend/bundle"
)

set_target_properties(main_app PROPERTIES
  OUTPUT_NAME "Voices of the Void Launcher"
)

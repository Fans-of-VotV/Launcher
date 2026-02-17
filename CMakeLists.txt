cmake_minimum_required(VERSION 3.18)

include(FetchContent)

project(VotV-Community-Launcher
  VERSION 0.1.0.0
  LANGUAGES CXX
)

FetchContent_Declare(
  webview
  GIT_REPOSITORY https://github.com/webview/webview
  GIT_TAG        0.12.0
)

FetchContent_MakeAvailable(webview)

file(GLOB_RECURSE SOURCES "Source/*.cpp" "Source/*.hpp")
add_executable(main_app WIN32 ${SOURCES})

target_link_libraries(main_app PRIVATE
  webview::core
)

set_target_properties(main_app PROPERTIES
  CXX_STANDARD 20
  OUTPUT_NAME "Voices of the Void Launcher"
)

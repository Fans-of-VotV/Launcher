cmake_minimum_required(VERSION 3.18)

project(VotV-Community-Launcher
  VERSION 0.1.0.0
  LANGUAGES CXX
)

file(GLOB_RECURSE SOURCES "Source/*.cpp" "Source/*.hpp")
add_executable(main_app WIN32 ${SOURCES})

set_target_properties(main_app PROPERTIES
  CXX_STANDARD 20
  OUTPUT_NAME "Voices of the Void Launcher"
)

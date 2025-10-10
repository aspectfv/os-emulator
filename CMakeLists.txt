cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(APPLE)
  if(EXISTS "/opt/homebrew/opt/llvm/bin/clang")
    set(CMAKE_C_COMPILER "/opt/homebrew/opt/llvm/bin/clang")
  endif()
  if(EXISTS "/opt/homebrew/opt/llvm/bin/clang++")
    set(CMAKE_CXX_COMPILER "/opt/homebrew/opt/llvm/bin/clang++")
  endif()
endif()

project(OSEmulator VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

include_directories(OSEmulator PRIVATE include)

add_executable(OSEmulator
  src/main.cpp
  src/Config.cpp
)

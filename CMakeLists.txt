cmake_minimum_required(VERSION 3.10)

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

include_directories(OSEmulator PRIVATE include)

add_executable(OSEmulator
  src/main.cpp
)

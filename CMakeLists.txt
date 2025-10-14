cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(APPLE)
  set(CMAKE_C_COMPILER "/opt/homebrew/opt/llvm/bin/clang")
  set(CMAKE_CXX_COMPILER "/opt/homebrew/opt/llvm/bin/clang++")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -L/opt/homebrew/opt/llvm/lib/c++ -I/opt/homebrew/opt/llvm/include/c++/v1")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,/opt/homebrew/opt/llvm/lib")
endif()

project(OSEmulator VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(OSEmulator
  src/main.cpp
  src/Config.cpp
  src/Emulator.cpp
  src/Process.cpp
  src/CommandParser.cpp
  src/CPUCore.cpp
  src/schedulers/FCFSScheduler.cpp
  src/schedulers/RRScheduler.cpp
  src/schedulers/SchedulerFactory.cpp
  src/instructions/Print.cpp
  src/instructions/Declare.cpp
  src/instructions/Arithmetic.cpp
  src/instructions/Sleep.cpp
  src/instructions/For.cpp
  src/instructions/InstructionFactory.cpp
  src/Utils.cpp

)

target_include_directories(OSEmulator PRIVATE include)

## Entry Point

The main entry point of the application is the `src/main.cpp` file.

## Prerequisites

To build and run this project, you will need:
*   A C++ compiler that supports C++20 (e.g., GCC, Clang).
*   CMake (version 3.10 or higher).

## Building the Project

The project uses CMake for its build system. Execute the following commands from the root directory of the repository:

```bash
# 1. Generate build files in a 'build' directory
cmake -S . -B build

# 2. Compile the project (assuming a Unix-like environment or Ninja/Make setup)
cd build
cmake --build .

# 3. Return to the root directory
cd .. 
```

The compiled executable, named `OSEmulator`, will be placed in the root directory.

## Running the Emulator

To start the emulator, run the executable from the root directory:

```bash
./OSEmulator
```

The emulator will load its configuration from `config.txt` and provide an interactive shell (`root:>`) where you can enter commands:

1.  **Initialize the emulator (loads config, sets up cores and scheduler):**
    ```
    root:> initialize
    ```
2.  **Start the scheduler (to automatically generate and run batch processes):**
    ```
    root:> scheduler-start
    ```
3.  **Start a new process and switch to its screen (e.g., process 'myprocess'):**
    ```
    root:> screen -s myprocess
    ```
4.  **List running and finished processes/screens:**
    ```
    root:> screen -ls
    ```
5.  **View detailed information (logs, state) of the current process screen:**
    ```
    root:> process-smi
    ```
6.  **Exit the current screen or the emulator:**
    ```
    root:> exit
    ```

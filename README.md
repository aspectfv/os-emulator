# OSEmulator

A C++ based Operating System Emulator that simulates CPU scheduling (Round Robin / FCFS), process management, and paged memory management with a backing store.

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
cmake --build build

```

The compiled executable, named `OSEmulator`, will be placed in the root directory.

## Configuration

The emulator behaves according to settings defined in `config.txt`. Key configurations include:

*   **CPU/Scheduler**: `num-cpu`, `scheduler`, `quantum-cycles`, `delay-per-exec`.
*   **Process Generation**: `batch-process-freq`, `min-ins`, `max-ins`.
*   **Memory Management**: 
    *   `max-overall-mem`: Total physical memory available.
    *   `mem-per-frame`: Size of a single memory frame.
    *   `min-mem-per-proc` / `max-mem-per-proc`: Memory allocation range for generated processes.

## Running the Emulator

To start the emulator, run the executable from the root directory:

```bash
./OSEmulator
```

The emulator provides an interactive shell (`root:>`). Below is the list of available commands.

### General Commands

| Command | Description |
| :--- | :--- |
| `initialize` | Initializes the emulator. Loads configuration from `config.txt`, creates CPU cores, sets up the scheduler, and initializes the Memory Manager (including backing store). |
| `exit` | **If in Root Shell:** Terminates the emulator.<br>**If in Process Screen:** Detaches from the current process and returns to the Root Shell. |

### Scheduler Control

| Command | Description |
| :--- | :--- |
| `scheduler-start` | Starts the scheduler. This will begin executing the instruction cycles on the CPU cores and automatically generate batch processes based on the config. |
| `scheduler-stop` | Stops the scheduler execution. |

### Process & Screen Management

| Command | Description |
| :--- | :--- |
| `screen -s <name> <mem_size>` | Creates a new process with the specified `<name>` and allocates `<mem_size>` bytes of memory. The process is immediately added to the ready queue. |
| `screen -c <name> <mem_size> <instructions>` | Creates a custom process with specific instructions. Instructions are semicolon-separated (e.g., `DECLARE x 1; ADD result x 5; PRINT result`). |
| `screen -r <name>` | Reattaches to a running or waiting process identified by `<name>`. Displays the process's execution logs. |
| `screen -ls` | Lists the status of all processes (CPU utilization, running processes, and finished processes). |

### System Monitoring & Statistics

| Command | Description |
| :--- | :--- |
| `report-util` | Generates a CPU utilization report. Prints details to the console and saves a copy to `csopesy-log.txt`. |
| `process-smi` | Displays a summary of system resources, including:<br>- CPU Utilization %<br>- Memory Usage (Used / Total bytes and Utilization %)<br>- List of running processes and their individual memory footprint. |
| `vmstat` | Displays Virtual Memory Statistics:<br>- Total, Used, and Free Physical Memory.<br>- CPU Tick counts (Idle vs Active).<br>- Paged In / Paged Out counts (Backing Store activity). |

## Example Usage

1.  **Initialize the system:**
    ```
    root:> initialize
    ```
2.  **Start the scheduler:**
    ```
    root:> scheduler-start
    ```
3.  **Create a process named `p1` with 4096 bytes of memory:**
    ```
    root:> screen -s p1 4096
    ```
4.  **Check memory and paging stats:**
    ```
    root:> vmstat
    ```
5.  **View detailed process info:**
    ```
    root:> process-smi
    ```

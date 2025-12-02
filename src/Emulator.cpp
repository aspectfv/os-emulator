#include "Emulator.hpp"
#include "Process.hpp"
#include "instructions/InstructionFactory.hpp"
#include "schedulers/SchedulerFactory.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdexcept>

Emulator::Emulator() {
  parser_.registerCommand(
      "initialize",
      [this](const std::vector<std::string> &args) { this->initialize(); });

  parser_.registerCommand(
      "exit", [this](const std::vector<std::string> &args) { this->exit(); });

  parser_.registerCommand(
      "screen",
      [this](const std::vector<std::string> &args) { this->screen(args); });

  parser_.registerCommand("scheduler-start",
                          [this](const std::vector<std::string> &args) {
                            this->scheduler_start();
                          });

  parser_.registerCommand(
      "scheduler-stop",
      [this](const std::vector<std::string> &args) { this->scheduler_stop(); });

  parser_.registerCommand(
      "report-util",
      [this](const std::vector<std::string> &args) { this->report_util(); });

  parser_.registerCommand(
      "process-smi",
      [this](const std::vector<std::string> &args) { this->process_smi(); });
  parser_.registerCommand(
      "vmstat",
      [this](const std::vector<std::string> &args) { this->vmstat(); });
}

bool Emulator::process_input(const std::string &input) {
  try {
    parser_.executeCommand(input);
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;

    if (e.what() == std::string("Exiting Emulator..."))
      return false;
  }

  return true;
}

void Emulator::cycle(std::stop_token st) {
  while (!st.stop_requested()) {
    int busy_cores = 0;
    uint32_t available_frames = memory_manager_->get_free_frames_size();
    uint32_t max_concurrent_processes = std::max(1u, available_frames);
    uint32_t current_running_processes = 0;

    for (auto &core : cores_) {
      if (!core.is_idle())
        current_running_processes++;
    }

    // assign processes to idle cores
    busy_cores =
        assign_processes(current_running_processes, max_concurrent_processes) +
        current_running_processes;

    // update cpu stats
    total_cpu_ticks_ += cores_.size(); // each core = one tick
    active_cpu_ticks_ += busy_cores;
    idle_cpu_ticks_ += (cores_.size() - busy_cores);

    // tick all cores and collect returned processes
    std::vector<Process *> returned_processes = tick_cores();

    // generate new processes if scheduler is .get()running
    if (scheduler_->is_running())
      generate_processes();

    // handle returned processes
    handle_returned_processes(returned_processes);

    // handle sleeping processes
    handle_sleeping_processes();

    available_frames = memory_manager_->get_free_frames_size();
    max_concurrent_processes = std::max(1u, available_frames);
    current_running_processes = 0;

    for (auto &core : cores_) {
      if (!core.is_idle())
        current_running_processes++;
    }

    assign_processes(current_running_processes, max_concurrent_processes);

    // sleep to prevent process from terminating too fast for debugging
    // std::this_thread::sleep_for(std::chrono::milliseconds(300)); // commented
    // out in actual demo for accuracy purposes
    cpu_cycles_++;

    // cycle finished -> notify waiting thread
    {
      std::lock_guard<std::mutex> lock(mtx_);
      cycle_finished_ = true;
      cv_.notify_one();
    }
  }
}

uint32_t Emulator::assign_processes(uint32_t current_running_processes,
                                    uint32_t max_concurrent_processes) {
  uint32_t assigned_processes = 0;

  for (size_t i = 0;
       i < cores_.size() && current_running_processes + assigned_processes <
                                max_concurrent_processes;
       ++i) {
    if (cores_[i].is_idle() && scheduler_->has_processes()) {
      Process *next_process = scheduler_->get_next_process();

      if (next_process) {
        bool is_registered =
            memory_manager_->is_process_registered(next_process->get_id());

        if (is_registered) {
          // registered process alreaady has memory allocated
          cores_[i].set_current_process(next_process);
          assigned_processes++;
        } else {
          // check new process memory
          uint32_t free_memory = memory_manager_->get_free_memory_size();
          uint32_t process_memory_size = next_process->get_total_memory_size();

          if (free_memory > process_memory_size) {
            // enough memory, assign process
            cores_[i].set_current_process(next_process);
            assigned_processes++;
          } else {
            // not enough memory, schedule for later
            scheduler_->add_process(next_process, true);
          }
        }
      }
    }
  }

  return assigned_processes;
}

std::vector<Process *> Emulator::tick_cores() {
  std::vector<Process *> returned_processes(cores_.size());

  // tick all cores in parallel
  // auto join after scope ends
  {
    std::vector<std::jthread> core_threads;
    core_threads.reserve(cores_.size());

    for (size_t i = 0; i < cores_.size(); ++i) {
      if (!cores_[i].is_idle()) {
        core_threads.emplace_back([this, i, &returned_processes]() {
          returned_processes[i] = cores_[i].tick(memory_manager_.get());
        });
      }
    }
  }

  return returned_processes;
}

void Emulator::generate_processes() {
  int batch_freq = config_.get_batch_process_freq();

  // generate batch_freq amount of processes
  for (int i = 0; i < batch_freq; ++i) {
    std::string process_name = "p" + std::to_string(process_count_++);

    int num_instructions =
        config_.get_min_ins() +
        (rand() % (config_.get_max_ins() - config_.get_min_ins() + 1));

    /* std::vector<std::unique_ptr<IInstruction>> instructions =
         InstructionFactory::create_instructions(process_name,
       num_instructions, config_.get_max_ins(), config_.get_min_ins());
   */

    std::vector<std::unique_ptr<IInstruction>> instructions =
        InstructionFactory::create_mo1_demo_instructions(process_name,
                                                         num_instructions);

    std::unique_ptr<Process> process = std::make_unique<Process>(
        process_name, num_instructions, config_.get_quantum_cycles());

    process->set_instructions(std::move(instructions));

    uint32_t min_mem_per_process = config_.get_min_mem_per_proc();
    uint32_t max_mem_per_process = config_.get_max_mem_per_proc();

    uint32_t random_memory_size =
        min_mem_per_process +
        (rand() % (max_mem_per_process - min_mem_per_process + 1));

    if (memory_manager_) {
      memory_manager_->register_process(process.get(), random_memory_size,
                                        config_.get_mem_per_frame());
    }

    processes_[process_name] = std::move(process);
    scheduler_->add_process(processes_[process_name].get());
  }
}

void Emulator::handle_returned_processes(
    const std::vector<Process *> &returned_processes) {

  for (size_t i = 0; i < returned_processes.size(); ++i) {
    Process *returned_process = returned_processes[i];

    if (!returned_process)
      continue;

    switch (returned_process->get_state()) {
    case Process::ProcessState::TERMINATED:
      terminated_processes_.push_back(returned_process);

      if (memory_manager_) {
        memory_manager_->remove_process(returned_process->get_id());
      }
      break;
    case Process::ProcessState::READY: {
      scheduler_->add_process(returned_process);

      if (cores_[i].is_idle() && scheduler_->has_processes()) {
        Process *next_process = scheduler_->get_next_process();

        if (next_process) {
          bool is_registered =
              memory_manager_->is_process_registered(next_process->get_id());

          if (is_registered) {
            cores_[i].set_current_process(next_process);
          } else {
            uint32_t free_memory = memory_manager_->get_free_memory_size();
            uint32_t process_memory_size =
                next_process->get_total_memory_size();

            if (free_memory > process_memory_size) {
              cores_[i].set_current_process(next_process);
            } else {
              scheduler_->add_process(next_process, true);
            }
          }
        }
      }

      break;
    }
    case Process::ProcessState::SLEEPING:
      sleeping_processes_.push_back(returned_process);
      break;
    default:
      break;
    }
  }
}

void Emulator::handle_sleeping_processes() {
  auto it = std::remove_if(sleeping_processes_.begin(),
                           sleeping_processes_.end(), [this](Process *process) {
                             process->decrement_sleep_ticks();
                             if (process->get_state() ==
                                 Process::ProcessState::READY) {
                               scheduler_->add_process(process, true);
                               return true;
                             }
                             return false;
                           });
  sleeping_processes_.erase(it, sleeping_processes_.end());
}

void Emulator::initialize() {
  if (is_initialized_)
    throw std::runtime_error("Emulator is already initialized.");

  std::string config_file = "config.txt";

  if (!config_.load(config_file))
    throw std::runtime_error("Configuration not loaded.");

  for (int i = 0; i < config_.get_num_cpu(); ++i) {
    cores_.emplace_back(i, config_.get_scheduler(),
                        config_.get_delay_per_exec());
  }

  std::string scheduler = config_.get_scheduler();
  scheduler_ = SchedulerFactory::create_scheduler(scheduler,
                                                  config_.get_quantum_cycles());

  memory_manager_ = std::make_unique<MemoryManager>(
      config_.get_max_overall_mem(), config_.get_mem_per_frame(),
      "csopesy-backing-store.txt");

  is_initialized_ = true;
  cycle_thread_ = std::jthread([this](std::stop_token st) { cycle(st); });
}

void Emulator::exit() {
  if (current_process_) {
    std::cout << "Exiting screen for process: " << current_process_->get_name()
              << std::endl;
    current_process_ = nullptr;
    return;
  }

  throw std::runtime_error("Exiting Emulator...");
}

void Emulator::screen(const std::vector<std::string> &args) {
  if (!is_initialized_)
    throw std::runtime_error("Emulator is not initialized.");

  if (args.empty())
    throw std::runtime_error("No screen command provided.");

  std::string arg = args[0];

  if (arg == "-s") {
    start_screen(const_cast<std::vector<std::string> &>(args));
    return;
  }

  if (arg == "-ls") {
    list_screens();
    return;
  }

  if (arg == "-r") {
    resume_screen(const_cast<std::vector<std::string> &>(args));
    return;
  }

  if (arg == "-c") {
    custom_screen(const_cast<std::vector<std::string> &>(args));
  }
}

void Emulator::start_screen(std::vector<std::string> &args) {
  if (args.size() < 2)
    throw std::runtime_error("No process name provided for -s command.");

  if (args.size() < 3)
    throw std::runtime_error("No process memory size provided for -s command.");

  std::string process_name = args[1];
  uint32_t memory_size = std::stoul(args[2]);

  auto [it, inserted] = processes_.try_emplace(process_name, nullptr);

  if (inserted) {
    int num_instructions =
        config_.get_min_ins() +
        (rand() % (config_.get_max_ins() - config_.get_min_ins() + 1));

    /* std::vector<std::unique_ptr<IInstruction>> instructions =
        InstructionFactory::create_instructions(process_name,
       num_instructions, config_.get_max_ins(), config_.get_min_ins());
    */

    std::vector<std::unique_ptr<IInstruction>> instructions =
        InstructionFactory::create_mo1_demo_instructions(process_name,
                                                         num_instructions);

    std::unique_ptr<Process> new_process = std::make_unique<Process>(
        process_name, num_instructions, config_.get_quantum_cycles());

    new_process->set_instructions(std::move(instructions));

    it->second = std::move(new_process);
  }

  Process *process = it->second.get();

  process->set_state(Process::ProcessState::READY);

  current_process_ = processes_[process_name].get();

  if (memory_manager_) {
    memory_manager_->register_process(current_process_, memory_size,
                                      config_.get_mem_per_frame());
  }

  scheduler_->add_process(current_process_);

  std::cout << "\033[2J\033[1;1H";
}

void Emulator::list_screens() {
  // wait for cpu cycle to finish for accurate report
  {
    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait(lock, [this] { return cycle_finished_; });
    cycle_finished_ = false;
  }
  // does the same as report_util but doesnt log to a file
  log_cpu_util_report(std::cout);
}

void Emulator::resume_screen(std::vector<std::string> &args) {
  if (args.size() < 2)
    throw std::runtime_error("No process name provided for -r command.");

  std::string process_name = args[1];

  auto it = processes_.find(process_name);

  if (it == processes_.end()) {
    std::string error_msg = "Process " + process_name + " not found.";
    throw std::runtime_error(error_msg);
    return;
  }

  current_process_ = it->second.get();

  std::cout << "\033[2J\033[1;1H";
}

void Emulator::custom_screen(std::vector<std::string> &args) {
  if (args.size() < 2)
    throw std::runtime_error("No process name provided for -c command.");

  std::string process_name = args[1];
  uint32_t memory_size = std::stoul(args[2]);
  std::string instructions_str = args[3];

  if (processes_.find(process_name) != processes_.end()) {
    std::string error_msg = "Process " + process_name + " already exists.";
    throw std::runtime_error(error_msg);
    return;
  }

  std::vector<std::unique_ptr<IInstruction>> instructions =
      InstructionFactory::create_instructions_from_string(instructions_str);

  if (instructions.empty() || instructions.size() > 50) {
    throw std::runtime_error(
        "Custom process must have between 1 and 50 instructions.");
  }

  std::unique_ptr<Process> process = std::make_unique<Process>(
      process_name, static_cast<int>(instructions.size()),
      config_.get_quantum_cycles());

  process->set_instructions(std::move(instructions));

  if (memory_manager_) {
    memory_manager_->register_process(process.get(), memory_size,
                                      config_.get_mem_per_frame());
  }

  processes_[process_name] = std::move(process);
  scheduler_->add_process(processes_[process_name].get());
}

void Emulator::scheduler_start() {
  if (!is_initialized_)
    throw std::runtime_error("Emulator is not initialized.");

  if (scheduler_ && scheduler_->is_running())
    throw std::runtime_error("Scheduler is already running.");

  scheduler_->start();

  std::cout << "Scheduler started." << std::endl;
}

void Emulator::scheduler_stop() {
  if (!is_initialized_)
    throw std::runtime_error("Emulator is not initialized.");

  scheduler_->stop();

  std::cout << "Scheduler stopped." << std::endl;
}

void Emulator::report_util() {
  if (!is_initialized_)
    throw std::runtime_error("Emulator is not initialized.");

  // wait for cpu cycle to finish for accurate report
  {
    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait(lock, [this] { return cycle_finished_; });
    cycle_finished_ = false;
  }

  std::ofstream report_file("csopesy-log.txt");

  if (!report_file.is_open())
    throw std::runtime_error("Failed to open report file.");

  int busy_cores = 0;

  for (const auto &core : cores_) {
    if (!core.is_idle())
      busy_cores++;
  }

  log_cpu_util_report(std::cout);
  log_cpu_util_report(report_file);

  report_file.close();
}

void Emulator::log_cpu_util_report(std::ostream &output_stream) {
  int busy_cores = 0;

  for (const auto &core : cores_) {
    if (!core.is_idle())
      busy_cores++;
  }

  int cpu_utilization = cores_.empty() ? 0 : (busy_cores * 100) / cores_.size();

  // output_stream << "CPU utilization: " << cpu_utilization << "%\n";
  output_stream << "CPU utilization: "
                << (scheduler_->is_running() ? cpu_utilization : 0) << "%\n";

  // output_stream << "Cores used: " << busy_cores << "\n";
  //
  output_stream << "Cores used: " << (scheduler_->is_running() ? busy_cores : 0)
                << "\n";

  // output_stream << "Cores available: " << cores_.size() - busy_cores <<
  // "\n\n";
  output_stream << "Cores available: "
                << (scheduler_->is_running() ? cores_.size() - busy_cores : 0)
                << "\n\n";
  output_stream << "----------------------------------------\n";
  output_stream << "Running processes:\n";

  bool any_running = false;

  auto format_process_log = [](const Process *process, int core_id) {
    std::string core_info =
        core_id < 0 ? "Finished" : "Core: " + std::to_string(core_id);
    return process->get_name() + "  " + "(" + process->get_created_at() +
           ")  " + core_info + "  " +
           std::to_string(process->get_instruction_pointer()) + "/" +
           std::to_string(process->get_total_instructions()) + "\n";
  };

  /* for (size_t i = 0; i < cores_.size(); ++i) {
    if (!cores_[i].is_idle()) {
      const Process *running_process = cores_[i].get_current_process();
      if (!running_process)
        continue;

      any_running = true;
      output_stream << format_process_log(running_process, i);
    }
  }
  */

  if (scheduler_->is_running()) {
    for (size_t i = 0; i < cores_.size(); ++i) {
      if (!cores_[i].is_idle()) {
        const Process *running_process = cores_[i].get_current_process();
        if (!running_process)
          continue;

        any_running = true;
        output_stream << format_process_log(running_process, i);
      }
    }
  }

  if (!any_running)
    output_stream << "No running processes.\n";

  output_stream << std::endl;

  output_stream << "Finished processes:\n";

  if (terminated_processes_.empty()) {
    output_stream << "No finished processes.\n";
  } else {

    /* for (const auto &finished_process : terminated_processes_) {
      output_stream << format_process_log(finished_process, -1);
    }
    */

    // used to limit log output to first 100 entries to avoid bloating the
    // file/screen
    int loop_count = terminated_processes_.size() <= 100
                         ? terminated_processes_.size()
                         : 100;

    for (int i = 0; i < loop_count; ++i) {
      const auto &finished_process = terminated_processes_[i];
      output_stream << format_process_log(finished_process, -1);
    }
  }

  output_stream << "----------------------------------------\n\n";
}

void Emulator::process_smi() {
  if (!is_initialized_)
    throw std::runtime_error("Emulator is not initialized.");

  // Wait for the current cycle to finish to ensure we get a consistent report
  {
    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait(lock, [this] { return cycle_finished_; });
    cycle_finished_ = false;
  }

  // 1. Calculate CPU Utilization
  int busy_cores = 0;
  for (const auto &core : cores_) {
    if (!core.is_idle()) {
      busy_cores++;
    }
  }

  // Avoid division by zero if no cores (unlikely if initialized)
  int cpu_utilization = cores_.empty() ? 0 : (busy_cores * 100) / cores_.size();

  // 2. Calculate Memory Usage
  // We use the MemoryManager to get accurate stats
  uint32_t total_mem = memory_manager_->get_total_memory_size();
  uint32_t used_mem = memory_manager_->get_used_memory_size();

  // Calculate percentage, utilizing uint64_t for multiplication to prevent
  // overflow
  int mem_utilization =
      total_mem > 0 ? static_cast<int>((static_cast<uint64_t>(used_mem) * 100) /
                                       total_mem)
                    : 0;

  // 3. Print Header and Stats
  std::cout << "\n--------------------------------------------" << std::endl;
  std::cout << "| PROCESS-SMI V01.00 Driver Version: 01.00 |" << std::endl;
  std::cout << "--------------------------------------------" << std::endl;
  std::cout << "CPU-Util: " << cpu_utilization << "%" << std::endl;
  std::cout << "Memory Usage: " << used_mem << "B / " << total_mem << "B"
            << std::endl;
  std::cout << "Memory Util: " << mem_utilization << "%" << std::endl;
  std::cout << "\n============================================" << std::endl;
  std::cout << "Running processes and memory usage:" << std::endl;
  std::cout << "--------------------------------------------" << std::endl;

  // 4. List Processes and their Memory Usage
  // Get the map of pid -> bytes used from MemoryManager
  std::unordered_map<int, uint32_t> process_mem_usage =
      memory_manager_->get_all_processes_memory_usage();

  // Iterate through our process list to match IDs with Names
  bool any_process_printed = false;
  for (const auto &pair : processes_) {
    const auto &process = pair.second;
    int pid = process->get_id();

    // Check if this process has allocated frames in memory
    if (process_mem_usage.find(pid) != process_mem_usage.end()) {
      // Only show processes that are currently holding memory
      // (This aligns with "Running processes and memory usage")
      std::cout << process->get_name() << " " << process_mem_usage[pid] << "B"
                << std::endl;
      any_process_printed = true;
    }
  }

  if (!any_process_printed) {
    std::cout << "No processes currently holding memory frames." << std::endl;
  }

  std::cout << "--------------------------------------------" << std::endl;
}

void Emulator::vmstat() {
  // wait for cpu cycle to finish for accurate report
  {
    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait(lock, [this] { return cycle_finished_; });
    cycle_finished_ = false;
  }

  if (!memory_manager_)
    throw std::runtime_error("Memory manager not initialized.");

  // memory stats
  uint32_t total_mem_size = memory_manager_->get_total_memory_size();
  uint32_t used_mem_size = memory_manager_->get_used_memory_size();
  uint32_t free_mem_size = memory_manager_->get_free_memory_size();

  // paging stats
  uint64_t paged_in_count = memory_manager_->get_paged_in_count();
  uint64_t paged_out_count = memory_manager_->get_paged_out_count();

  // display vmstat output
  std::cout << "Total Memory: " << total_mem_size << " bytes" << std::endl;
  std::cout << "Used Memory: " << used_mem_size << " bytes" << std::endl;
  std::cout << "Free Memory: " << free_mem_size << " bytes" << std::endl;
  std::cout << "Idle CPU Ticks: " << idle_cpu_ticks_ << std::endl;
  std::cout << "Active CPU Ticks: " << active_cpu_ticks_ << std::endl;
  std::cout << "Total CPU Ticks: " << total_cpu_ticks_ << std::endl;
  std::cout << "Num paged in: " << paged_in_count << std::endl;
  std::cout << "Num paged out: " << paged_out_count << std::endl;
}

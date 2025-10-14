#pragma once
#include "instructions/IInstruction.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

struct ProcessLog {
  std::string timestamp;
  int core_id;
  std::string message;
};

class Process {
public:
  enum class ProcessState { NEW, READY, RUNNING, WAITING, TERMINATED };

  Process(const std::string &name, const std::string &created_at,
          int total_instructions, int quantum_cycles);
  void execute_current_instruction(int cpu_core_id);
  const int get_id() const;
  const std::string get_name() const;
  const std::string get_created_at() const;
  const int get_total_instructions() const;
  const int get_instruction_pointer() const;
  const ProcessState get_state() const;
  const bool is_finished() const;
  const int is_quantum_expired() const;
  const std::vector<ProcessLog> &get_logs() const { return logs_; }

  void increment_instruction_pointer();
  void
  set_instructions(std::vector<std::unique_ptr<IInstruction>> &&instructions);
  void set_state(ProcessState state);
  void set_quantum_remaining(int quantum_cycles);
  void decrement_quantum_remaining();
  void print_instructions() const;

private:
  static int next_id_;
  int id_;
  std::string name_;
  std::string created_at_;
  std::vector<std::unique_ptr<IInstruction>> instructions_;
  std::unordered_map<std::string, int> symbol_table_;
  int total_instructions_;
  int instruction_pointer_;
  ProcessState state_;
  int quantum_remaining_;
  std::vector<ProcessLog> logs_;
};

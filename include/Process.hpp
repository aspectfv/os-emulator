#pragma once
#include "Utils.hpp"
#include "instructions/IInstruction.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class MemoryManager;

struct ProcessLog {
  std::string timestamp = Utils::current_timestamp();
  int core_id;
  std::string message;
};

struct PageTableEntry {
  bool valid_bit = false;
  bool dirty_bit = false;
  int frame_number = -1;
};

class Process {
public:
  enum class ProcessState { NEW, READY, RUNNING, SLEEPING, TERMINATED };

  Process(const std::string &name, int total_instructions, int quantum_cycles);
  void execute_current_instruction(int cpu_core_id,
                                   MemoryManager *memory_manager);
  const int get_id() const;
  const std::string get_name() const;
  const std::string get_created_at() const;
  const int get_total_instructions() const;
  const int get_instruction_pointer() const;
  const ProcessState get_state() const;
  const int is_quantum_expired() const;
  const std::vector<ProcessLog> &get_logs() const { return logs_; }
  const int get_page_table_size() const;
  const PageTableEntry &get_page_table_entry(int virtual_page_number) const;
  PageTableEntry &get_page_table_entry(int virtual_page_number);
  const uint32_t get_total_memory_size() const;
  const uint32_t get_backing_store_offset() const;
  const bool get_access_violation() const;

  void increment_instruction_pointer();
  void
  set_instructions(std::vector<std::unique_ptr<IInstruction>> &&instructions);
  void set_state(ProcessState state);
  void set_quantum_remaining(int quantum_cycles);
  void decrement_quantum_remaining();
  void decrement_sleep_ticks();
  void set_total_memory_size(uint32_t size);
  void set_backing_store_offset(uint32_t offset);
  void set_access_violation(bool violation);

private:
  static int next_id_;
  int id_;
  std::string name_;
  std::string created_at_ = Utils::current_timestamp();
  std::vector<std::unique_ptr<IInstruction>> instructions_;
  std::unordered_map<std::string, uint16_t> symbol_table_;
  uint32_t next_symbol_address_ = 0x40; // start at 64 bytes
  int total_instructions_;
  int instruction_pointer_ = 0;
  ProcessState state_;
  int quantum_remaining_;
  std::vector<ProcessLog> logs_;
  int sleep_ticks_;

  // page table for memory management
  std::vector<PageTableEntry> page_table_;

  uint32_t total_memory_size_ = 0;
  uint32_t backing_store_offset_ = 0;

  bool access_violation_ = false;

  // callback definitions
  void add_log(int cpu_core_id, const std::string &message);
  uint16_t get_variable(const std::string &var_name,
                        MemoryManager *memory_manager);
  void add_variable(std::pair<std::string, uint16_t> var);
  void add_instructions(
      std::vector<std::unique_ptr<IInstruction>> &&new_instructions);
  void sleep(int ticks);
};

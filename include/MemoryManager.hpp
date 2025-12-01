#pragma once

#include "Process.hpp"
#include <fstream>
#include <mutex>
#include <queue>
#include <unordered_map>
#include <vector>

struct FrameTableEntry {
  bool is_allocated = false;
  int owner_process_id = -1;
  uint32_t virtual_page_number = -1;
};

enum class MemoryAccessResult { SUCCESS, ACCESS_VIOLATION, ERROR };

class MemoryManager {
public:
  MemoryManager(uint32_t physical_memory_size, uint32_t mem_per_frame,
                const std::string &backing_store_filename);

  MemoryAccessResult read(uint32_t virtual_address, Process *process,
                          uint16_t &out_value);
  MemoryAccessResult write(uint32_t virtual_address, Process *process,
                           uint16_t value);

  void register_process(Process *process, uint32_t size,
                        uint32_t mem_per_frame);

  void remove_process(int process_id);
  bool is_process_registered(int process_id) const;

  // stats
  uint32_t get_total_memory_size() const;
  uint32_t get_used_memory_size() const;
  uint32_t get_free_memory_size() const;
  uint32_t get_total_memory_usage() const;
  uint32_t get_free_frames_size() const;
  uint32_t get_process_memory_usage(int process_id) const;
  std::unordered_map<int, uint32_t> get_all_processes_memory_usage() const;
  uint64_t get_paged_in_count() const;
  uint64_t get_paged_out_count() const;

private:
  mutable std::mutex memory_mutex_;

  std::vector<char> physical_memory_;
  std::vector<FrameTableEntry> frame_table_;
  std::queue<uint32_t> free_frame_list_;
  uint32_t mem_per_frame_;

  // fifo page replacement
  std::queue<uint32_t> active_frame_queue_;

  // backing store
  std::fstream backing_store_;
  long long next_backing_store_pos = 0;

  // process tracker
  std::unordered_map<int, Process *> processes_;

  uint64_t paged_in_count_ = 0;
  uint64_t paged_out_count_ = 0;
  uint64_t total_memory_usage_ = 0;

  // helpers
  void page_fault(Process *process, uint32_t virtual_page_number);
  int get_victim_frame();
  void read_page_from_backing_store(Process *process,
                                    uint32_t virtual_page_number,
                                    int frame_number);
  void write_page_to_backing_store(Process *process,
                                   uint32_t virtual_page_number,
                                   int frame_number);
};

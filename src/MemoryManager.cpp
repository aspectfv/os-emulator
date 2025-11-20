#include "MemoryManager.hpp"
#include "Process.hpp"

MemoryManager::MemoryManager(uint32_t max_overall_mem, uint32_t mem_per_frame,
                             const std::string &backing_store_filename)
    : physical_memory_(max_overall_mem, 0),
      frame_table_(max_overall_mem / mem_per_frame),
      mem_per_frame_(mem_per_frame),
      backing_store_(backing_store_filename, std::ios::in | std::ios::out |
                                                 std::ios::binary |
                                                 std::ios::trunc) {
  if (!backing_store_.is_open()) {
    throw std::runtime_error("Failed to open backing store file.");
  }

  // initialize free frame list
  for (uint32_t i = 0; i < frame_table_.size(); ++i) {
    free_frame_list_.push(i);
  }
}

MemoryAccessResult MemoryManager::read(uint32_t virtual_address,
                                       Process *process, uint16_t &out_value) {
  std::lock_guard<std::mutex> lock(memory_mutex_);

  // calculate virtual page number and offset
  uint32_t virtual_page_number = virtual_address / mem_per_frame_;
  uint32_t offset = virtual_address % mem_per_frame_;

  if (virtual_page_number >= process->get_page_table_size()) {
    out_value = 0;
    return MemoryAccessResult::ACCESS_VIOLATION;
  }

  // get page table entry
  const PageTableEntry &page_table_entry =
      process->get_page_table_entry(virtual_page_number);

  // if valid bit is false, it is a page fault
  if (page_table_entry.valid_bit == false) {
    page_fault_(process, virtual_page_number);
    paged_in_count_++;
  }

  uint32_t physical_address =
      page_table_entry.frame_number * mem_per_frame_ + offset;

  if (physical_address + sizeof(uint16_t) > physical_memory_.size()) {
    out_value = 0;
    return MemoryAccessResult::ERROR;
  }

  out_value =
      *reinterpret_cast<uint16_t *>(&physical_memory_[physical_address]);

  return MemoryAccessResult::SUCCESS;
}

MemoryAccessResult MemoryManager::write(uint32_t virtual_address,
                                        Process *process, uint16_t value) {
  std::lock_guard<std::mutex> lock(memory_mutex_);

  // calculate virtual page number and offset
  uint32_t virtual_page_number = virtual_address / mem_per_frame_;
  uint32_t offset = virtual_address % mem_per_frame_;

  if (virtual_page_number >= process->get_page_table_size()) {
    return MemoryAccessResult::ACCESS_VIOLATION;
  }

  PageTableEntry &page_table_entry =
      process->get_page_table_entry(virtual_page_number);

  if (page_table_entry.valid_bit == false) {
    page_fault_(process, virtual_page_number);
    paged_in_count_++;
  }

  uint32_t physical_address =
      page_table_entry.frame_number * mem_per_frame_ + offset;

  if (physical_address + sizeof(uint16_t) > physical_memory_.size()) {
    return MemoryAccessResult::ERROR;
  }

  reinterpret_cast<uint16_t *>(&physical_memory_[physical_address])[0] = value;
  page_table_entry.dirty_bit = true;

  return MemoryAccessResult::SUCCESS;
}

void MemoryManager::page_fault_(Process *process,
                                uint32_t virtual_page_number) {
  // Page fault handling logic goes here
  int frame_number;

  if (!free_frame_list_.empty()) {
    // pop from free frame list
    frame_number = free_frame_list_.front();
    free_frame_list_.pop();
  } else {
    // get victim frame with FIFO
    frame_number = get_victim_frame_();
  }

  // allocate frame
  frame_table_[frame_number].is_allocated = true;
  frame_table_[frame_number].owner_process_id = process->get_id();
  frame_table_[frame_number].virtual_page_number = virtual_page_number;
}

int MemoryManager::get_victim_frame_() {
  if (active_frame_queue_.empty()) {
    return 0;
  }

  int victim_frame = active_frame_queue_.front();
  active_frame_queue_.pop();

  // if victim frame is allocated, page out
  if (frame_table_[victim_frame].is_allocated) {
    int owner_process_id = frame_table_[victim_frame].owner_process_id;
    int virtual_page_number = frame_table_[victim_frame].virtual_page_number;

    // check for dirty bit and write to backing store if necessary
    auto it = processes_.find(owner_process_id);
    if (it != processes_.end()) {
      Process *owner_process = it->second;

      if (virtual_page_number < owner_process->get_page_table_size()) {
        auto &page_table_entry =
            owner_process->get_page_table_entry(virtual_page_number);

        if (page_table_entry.dirty_bit) {
          write_to_backing_store_(owner_process, virtual_page_number,
                                  victim_frame);
          paged_out_count_++;
        }

        page_table_entry.valid_bit = false;
        page_table_entry.dirty_bit = false;
        page_table_entry.frame_number = -1;
      }
    }

    // clear frame table entry
    frame_table_[victim_frame].is_allocated = false;
    frame_table_[victim_frame].owner_process_id = -1;
    frame_table_[victim_frame].virtual_page_number = -1;
  }

  return victim_frame;
}

void MemoryManager::write_to_backing_store_(Process *process,
                                            uint32_t virtual_page_number,
                                            int frame_number) {
  if (!backing_store_.is_open()) {
    throw std::runtime_error("Backing store file is not open.");
  }

  uint32_t backing_store_offset = process->get_backing_store_offset() +
                                  virtual_page_number * mem_per_frame_;

  uint32_t frame_start_address = frame_number * mem_per_frame_;

  backing_store_.seekp(backing_store_offset, std::ios::beg);
  backing_store_.write(&physical_memory_[frame_start_address], mem_per_frame_);
  backing_store_.flush();

  if (backing_store_.fail()) {
    throw std::runtime_error("Failed to write to backing store.");
  }
}

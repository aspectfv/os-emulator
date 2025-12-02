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
    page_fault(process, virtual_page_number);
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
    page_fault(process, virtual_page_number);
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

void MemoryManager::register_process(Process *process, uint32_t size,
                                     uint32_t mem_per_frame) {
  std::lock_guard<std::mutex> lock(memory_mutex_);

  process->set_total_memory_size(size);
  process->set_backing_store_offset(next_backing_store_pos);
  next_backing_store_pos += size;

  uint32_t num_pages =
      (size + mem_per_frame - 1) / mem_per_frame; // ceiling division
  process->set_page_table_size(num_pages);

  uint32_t actual_memory_allocated = num_pages * mem_per_frame;
  total_memory_usage_ += actual_memory_allocated;

  processes_[process->get_id()] = process;
}

void MemoryManager::remove_process(int process_id) {
  std::lock_guard<std::mutex> lock(memory_mutex_);

  for (auto &frame_entry : frame_table_) {
    if (frame_entry.is_allocated &&
        frame_entry.owner_process_id == process_id) {
      frame_entry.is_allocated = false;
      frame_entry.owner_process_id = -1;
      frame_entry.virtual_page_number = -1;

      free_frame_list_.push(&frame_entry - &frame_table_[0]); // frame index
    }
  }

  auto it = processes_.find(process_id);
  if (it != processes_.end()) {
    processes_.erase(it);
  }
}

bool MemoryManager::is_process_registered(int process_id) const {
  std::lock_guard<std::mutex> lock(memory_mutex_);
  return processes_.find(process_id) != processes_.end();
}

uint32_t MemoryManager::get_total_memory_size() const {
  std::lock_guard<std::mutex> lock(memory_mutex_);
  return static_cast<uint32_t>(physical_memory_.size());
}

uint32_t MemoryManager::get_used_memory_size() const {
  std::lock_guard<std::mutex> lock(memory_mutex_);

  uint32_t used_frames = 0;

  for (const auto &frame_entry : frame_table_) {
    if (frame_entry.is_allocated) {
      used_frames++;
    }
  }

  return used_frames * mem_per_frame_;
}

uint32_t MemoryManager::get_free_memory_size() const {
  return get_total_memory_size() - get_used_memory_size();
}

uint32_t MemoryManager::get_total_memory_usage() const {
  std::lock_guard<std::mutex> lock(memory_mutex_);
  return static_cast<uint32_t>(total_memory_usage_);
}

uint32_t MemoryManager::get_free_frames_size() const {
  std::lock_guard<std::mutex> lock(memory_mutex_);
  return static_cast<uint32_t>(free_frame_list_.size());
}

uint32_t MemoryManager::get_process_memory_usage(int process_id) const {
  std::lock_guard<std::mutex> lock(memory_mutex_);

  uint32_t used_frames = 0;

  for (const auto &frame_entry : frame_table_) {
    if (frame_entry.is_allocated &&
        frame_entry.owner_process_id == process_id) {
      used_frames++;
    }
  }

  return used_frames * mem_per_frame_;
}

std::unordered_map<int, uint32_t>
MemoryManager::get_all_processes_memory_usage() const {
  std::lock_guard<std::mutex> lock(memory_mutex_);

  std::unordered_map<int, uint32_t> memory_usage_map;

  for (const auto &frame_entry : frame_table_) {
    if (frame_entry.is_allocated) {
      memory_usage_map[frame_entry.owner_process_id] += mem_per_frame_;
    }
  }

  return memory_usage_map;
}

uint64_t MemoryManager::get_paged_in_count() const {
  std::lock_guard<std::mutex> lock(memory_mutex_);
  return paged_in_count_;
}

uint64_t MemoryManager::get_paged_out_count() const {
  std::lock_guard<std::mutex> lock(memory_mutex_);
  return paged_out_count_;
}

void MemoryManager::page_fault(Process *process, uint32_t virtual_page_number) {
  int frame_number;

  if (!free_frame_list_.empty()) {
    // pop from free frame list
    frame_number = free_frame_list_.front();
    free_frame_list_.pop();
  } else {
    // get victim frame with FIFO
    frame_number = get_victim_frame();
  }

  // allocate frame
  frame_table_[frame_number].is_allocated = true;
  frame_table_[frame_number].owner_process_id = process->get_id();
  frame_table_[frame_number].virtual_page_number = virtual_page_number;

  read_page_from_backing_store(process, virtual_page_number, frame_number);

  // update process page table
  process->get_page_table_entry(virtual_page_number).valid_bit = true;
  process->get_page_table_entry(virtual_page_number).dirty_bit = false;
  process->get_page_table_entry(virtual_page_number).frame_number =
      frame_number;

  // always add newly allocated frame to the back of the active frame queue
  active_frame_queue_.push(frame_number);
}

int MemoryManager::get_victim_frame() {
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
          write_page_to_backing_store(owner_process, virtual_page_number,
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

void MemoryManager::read_page_from_backing_store(Process *process,
                                                 uint32_t virtual_page_number,
                                                 int frame_number) {
  if (!backing_store_.is_open()) {
    throw std::runtime_error("Backing store file is not open.");
  }

  uint32_t backing_store_offset = process->get_backing_store_offset() +
                                  virtual_page_number * mem_per_frame_;

  // get phyiscal memory address of the frame
  uint32_t frame_start_address = frame_number * mem_per_frame_;

  // 1. Zero out the frame first (Pre-fill with 0) using std::fill
  // This avoids C-style memset issues
  auto start_iter = physical_memory_.begin() + frame_start_address;
  std::fill(start_iter, start_iter + mem_per_frame_, 0);

  // clear any eof/fail flags before reading
  backing_store_.clear();

  // 2. Attempt to read from backing store
  backing_store_.seekg(backing_store_offset, std::ios::beg);
  backing_store_.read(&physical_memory_[frame_start_address], mem_per_frame_);

  // 3. Clear flags regardless of success/failure
  // If read failed (e.g. EOF because file is empty), gcount will be 0,
  // and our memory remains 0-filled from step 1.
  backing_store_.clear();
}

void MemoryManager::write_page_to_backing_store(Process *process,
                                                uint32_t virtual_page_number,
                                                int frame_number) {
  if (!backing_store_.is_open()) {
    throw std::runtime_error("Backing store file is not open.");
  }

  uint32_t backing_store_offset = process->get_backing_store_offset() +
                                  virtual_page_number * mem_per_frame_;

  // get phyiscal memory address of the frame
  uint32_t frame_start_address = frame_number * mem_per_frame_;

  backing_store_.seekp(backing_store_offset, std::ios::beg);
  backing_store_.write(&physical_memory_[frame_start_address], mem_per_frame_);
  backing_store_.flush();

  if (backing_store_.fail()) {
    throw std::runtime_error("Failed to write to backing store.");
  }
}

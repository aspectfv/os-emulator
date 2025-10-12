#include "instructions/Print.hpp"
#include "Process.hpp"

Print::Print(const std::string &msg) : message_(msg) {}

void Print::execute(Process *process, int cpu_core_id) {
  process->add_log("00:00:00", cpu_core_id, message_);
}

const std::string &Print::get_message() const { return message_; }

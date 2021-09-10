#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <linux_parser.h>
#include <iostream>
#include <algorithm>

#include "process.h"
#include "processor.h"
#include "system.h"

using namespace std;

System::System()
{
    os_ = LinuxParser::OperatingSystem();
    kernel_ = LinuxParser::Kernel();
    cpu_ = Processor();
}

// DONE: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// DONE: Return a container composed of the system's processes
vector<Process>& System::Processes() {
  processes_.clear();
  vector<int> pids = LinuxParser::Pids();
  for (int pid : pids)
  {
    Process proc = Process(pid);
    if (proc.CpuUtilization() > 0 && safe_convert<int>(proc.Ram()) > 0)
    {
    	processes_.push_back(proc);
    }
  }
  std::sort(processes_.begin(), processes_.end());
  return processes_;
}

// DONE: Return the system's kernel identifier (string)
std::string System::Kernel() { return kernel_; }

// DONE: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// DONE: Return the operating system name
std::string System::OperatingSystem() { return os_; }

// DONE: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// DONE: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// DONE: Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }
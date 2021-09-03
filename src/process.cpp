#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <linux_parser.h>

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid): pid{pid}
{
    user = LinuxParser::User(pid);
    cmd = LinuxParser::Command(pid);
}

// DONE: Return this process's ID
int Process::Pid() const { return pid; }

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() const {
    long total_time = LinuxParser::ActiveJiffies(Pid());
    float up_time = UpTime();
    if (up_time == 0)
    {
        return 0;
    }
    return (float) total_time / sysconf(_SC_CLK_TCK) / (float) up_time;
}

// DONE: Return the command that generated this process
string Process::Command() const { 
  if (cmd.size() > 40)
  {
    return cmd.substr(0, 40) + "...";
  }
  return cmd;
}

// DONE: Return this process's memory utilization
string Process::Ram() const { return LinuxParser::Ram(Pid()); }

// DONE: Return the user (name) that generated this process
string Process::User() const { return user; }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() const { return LinuxParser::UpTime(Pid()); }

// DONE: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
    return CpuUtilization() < a.CpuUtilization();
}
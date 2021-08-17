#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

string StringFromLine(string line, int index)
{
  string value;
  std::istringstream linestream(line);
  int i = 0;
  while (linestream >> value && i < index)
  {
    i++;
  }
  return value;
}

vector<string> VectorFromLine(string line)
{
  vector<string> values;
  string value;
  std::istringstream linestream(line);
  while (linestream >> value)
  {
    values.push_back(value);
  }
  return values;
}

string LineFromFile(string filename, string targetKey, int keyIndex = 0)
{
  string line;
  string key;
  std::ifstream filestream(filename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), ':', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');

      key = StringFromLine(line, keyIndex);
      if (key == targetKey) {
        return line;
      }
    }
  }
  return line;
}

string LineFromFile(string filename, int lineIndex)
{
  string line;
  std::ifstream stream(filename);
  if (stream.is_open()) {
    int i = 0;
    while (std::getline(stream, line) && i < lineIndex) {
      i++;
    }
  }
  return line;
}

vector<string> LineVectorFromFile(string filename, int lineIndex)
{
  string line = LineFromFile(filename, lineIndex);
  return VectorFromLine(line);
}

string StringFromFile(string filename, int valueIndex)
{
  string firstLine = LineFromFile(filename, 0);
  return StringFromLine(firstLine, valueIndex);
}

string StringFromFile(string filename, string targetKey, int keyIndex = 0, int valueIndex = 1)
{
  string line = LineFromFile(filename, targetKey, keyIndex);
  return StringFromLine(line, valueIndex);
}

template <typename Type>
Type safe_convert(string s)
{
  Type f = 0;
  try
  {
    f = (Type) stof(s);
  }
  catch(...) {}
  return f;
}

// --------------------------------------------------

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  return StringFromFile(kOSPath, "PRETTY_NAME");
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  return StringFromFile(kProcDirectory + kVersionFilename, 2);
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string filename = kProcDirectory + kMeminfoFilename;
  string memTotal = StringFromFile(filename, "MemTotal");
  string memFree  = StringFromFile(filename, "MemFree");
  float total = safe_convert<float>(memTotal);
  float free = safe_convert<float>(memFree);
  return (total - free) / total;
}

// DONE: Read and return the system uptime (in seconds)
long LinuxParser::UpTime() {
  string filename = kProcDirectory + kUptimeFilename;
  string upTime = StringFromFile(filename, 0);
  return safe_convert<long>(upTime);
}

/* 
0    1       2      3       4         5      6     7        9      9      109
     user    nice   system  idle      iowait irq   softirq  steal  guest  guest_nice
cpu  74608   2520   24433   1117073   6176   4054  0        0      0      0

Idle = idle + iowait
NonIdle = user + nice + system + irq + softirq + steal
Total = Idle + NonIdle
*/

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  vector<string> cpuUtils = CpuUtilization();
  long jiffies = 0;
  for (int i = 1; i < cpuUtils.size(); i++)
  {
    jiffies += safe_convert<long>(cpuUtils[i]);
  }
  return jiffies;
}

// DONE: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  string filename = kProcDirectory + to_string(pid) + kStatFilename;
  vector<string> fields = LineVectorFromFile(filename, 0);
  long utime = safe_convert<long>(fields[14-1]);
  long stime = safe_convert<long>(fields[15-1]);
  return utime + stime;
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  return Jiffies() - IdleJiffies();
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> cpuUtils = CpuUtilization();
  long jiffies = 0;
  jiffies += safe_convert<long>(cpuUtils[4]);
  jiffies += safe_convert<long>(cpuUtils[5]);
  return jiffies;
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string filename = kProcDirectory + kStatFilename;
  return LineVectorFromFile(filename, 0);
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string filename = kProcDirectory + kStatFilename;
  string processes = StringFromFile(filename, "processes");
  return safe_convert<int>(processes);
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string filename = kProcDirectory + kStatFilename;
  string processes = StringFromFile(filename, "procs_running");
  return safe_convert<int>(processes);
}

// DONE: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string filename = kProcDirectory + to_string(pid) + kCmdlineFilename;
  return LineFromFile(filename, 0);
}

// DONE: Read and return the memory used by a process (in MB)
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) {
  string filename = kProcDirectory + to_string(pid) + kStatusFilename;
  int ram = safe_convert<int>(StringFromFile(filename, "VmSize")) * 0.001;
  return to_string(ram);
}

// DONE: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string filename = kProcDirectory + to_string(pid) + kStatusFilename;
  return StringFromFile(filename, "Uid");
}

// DONE: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string uid = Uid(pid);
  return StringFromFile(kPasswordPath, uid, 2, 0);
}

// DONE: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  string filename = kProcDirectory + to_string(pid) + kStatFilename;
  string starttime = StringFromFile(filename, 22 - 1);
  float stime = safe_convert<float>(starttime);
  return UpTime() - (stime / sysconf(_SC_CLK_TCK));
}

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

string valueFromLine(string line, int index)
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

vector<string> vectorFromLine(string line)
{
  vector<string> values;
  string value;
  std::istringstream linestream(line);
  while (linestream >> value)
  {
    values.emplace_back(value);
  }
  return values;
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

struct File
{
  std::string filename;
  
  File(std::string fn): filename{fn} {}

  std::string findLine(std::string targetKey, int keyIndex = 0);
  std::string findLine(int lineIndex = 0);
  
  std::vector<std::string> findLineVector(int lineIndex = 0);

  std::string findValue(int valueIndex = 0);
  std::string findValue(std::string targetKey, int keyIndex = 0, int valueIndex = 1);
	
  template <typename Type>
  Type findNumValue(int valueIndex = 0);
  
  template <typename Type>
  Type findNumValue(std::string targetKey, int keyIndex = 0, int valueIndex = 1);
  
};

string File::findLine(string targetKey, int keyIndex)
{
  string line;
  string key;
  std::ifstream filestream(filename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), ':', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');

      key = valueFromLine(line, keyIndex);
      if (key == targetKey) {
        return line;
      }
    }
  }
  return line;
}

string File::findLine(int lineIndex)
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

vector<string> File::findLineVector(int lineIndex)
{
  return vectorFromLine(findLine(lineIndex));
}

string File::findValue(int valueIndex)
{
  return valueFromLine(findLine(), valueIndex);
}

string File::findValue(string targetKey, int keyIndex, int valueIndex)
{
  return valueFromLine(findLine(targetKey, keyIndex), valueIndex);
}

template <typename Type>
Type File::findNumValue(int valueIndex)
{
  return safe_convert<Type>(findValue(valueIndex));
}

template <typename Type>
Type File::findNumValue(std::string targetKey, int keyIndex, int valueIndex)
{
  return safe_convert<Type>(findValue(targetKey, keyIndex, valueIndex));
}

// --------------------------------------------------

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  File file{kOSPath};
  return file.findValue(filterPrettyName);
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  File file{kProcDirectory + kVersionFilename};
  return file.findValue(2);
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
        pids.emplace_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  File file{kProcDirectory + kMeminfoFilename};
  float total = file.findNumValue<float>(filterMemTotal);
  float free  = file.findNumValue<float>(filterMemFree);
  //float total = safe_convert<float>(memTotal);
  //float free = safe_convert<float>(memFree);
  return (total - free) / total;
}

// DONE: Read and return the system uptime (in seconds)
long LinuxParser::UpTime() {
  File file{kProcDirectory + kUptimeFilename};
  //string upTime = file.findValue();
  return file.findNumValue<long>();
}

/* 
     0       1      2       3         4      5     6        7      8      9
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
  for (size_t i = 1; i < cpuUtils.size(); i++)
  {
    jiffies += safe_convert<long>(cpuUtils[i]);
  }
  return jiffies;
}

// DONE: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  File file{kProcDirectory + to_string(pid) + kStatFilename};
  vector<string> fields = file.findLineVector();
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
  jiffies += safe_convert<long>(cpuUtils[CPUStates::kIdle_]);
  jiffies += safe_convert<long>(cpuUtils[CPUStates::kIOwait_]);
  return jiffies;
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  File file{kProcDirectory + kStatFilename};
  vector<string> cpuUtils = file.findLineVector();
  cpuUtils.erase(cpuUtils.begin());
  return cpuUtils;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  File file{kProcDirectory + kStatFilename};
  return file.findNumValue<int>(filterProcesses);
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  File file{kProcDirectory + kStatFilename};
  return file.findNumValue<int>(filterProcsRunning);
}

// DONE: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  File file{kProcDirectory + to_string(pid) + kCmdlineFilename};
  return file.findLine();
}

// DONE: Read and return the memory used by a process (in MB)
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  File file{kProcDirectory + to_string(pid) + kStatusFilename};
  int ram = file.findNumValue<int>(filterVmRSS) * 0.001;
  return to_string(ram);
}

// DONE: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  File file{kProcDirectory + to_string(pid) + kStatusFilename};
  return file.findValue(filterUid);
}

// DONE: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  File file{kPasswordPath};
  return file.findValue(Uid(pid), 2, 0);
}

// DONE: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  File file{kProcDirectory + to_string(pid) + kStatFilename};
  //string starttime = file.findValue(22 - 1);
  float stime = file.findNumValue<float>(22 - 1);
  return UpTime() - (stime / sysconf(_SC_CLK_TCK));
}

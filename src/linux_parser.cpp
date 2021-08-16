#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

string getFromLine(string line, int index)
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

string getLineFromFile(string filename, string targetKey, int keyIndex = 0)
{
  string line;
  string key;
  std::ifstream filestream(filename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), ':', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');

      key = getFromLine(line, keyIndex);
      if (key == targetKey) {
        std::replace(line.begin(), line.end(), '_', ' ');
        return line;
      }
    }
  }
  return line;
}

string getLineFromFile(string filename, int lineIndex)
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

vector<string> getLineVectorFromFile(string filename, int lineIndex)
{
  string line = getLineFromFile(filename, lineIndex);
  vector<string> values;
  string value;
  std::istringstream linestream(line);
  while (linestream >> value)
  {
    values.push_back(value);
  }
  return values;

}

string getValueFromFile(string filename, int valueIndex)
{
  string firstLine = getLineFromFile(filename, 0);
  return getFromLine(firstLine, valueIndex);
}

string getValueFromFile(string filename, string targetKey, int keyIndex = 0, int valueIndex = 1)
{
  string line = getLineFromFile(filename, targetKey, keyIndex);
  return getFromLine(line, valueIndex);
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  return getValueFromFile(kOSPath, "PRETTY_NAME");
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  return getValueFromFile(kProcDirectory + kVersionFilename, 2);
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
  string memTotal = getValueFromFile(filename, "MemTotal");
  string memFree  = getValueFromFile(filename, "MemFree");
  return stof(memTotal) - stof(memFree);
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() { 
  string filename = kProcDirectory + kUptimeFilename;
  string upTime = getValueFromFile(filename, 0);
  float upTimeFloat = stof(upTime);
  return upTimeFloat / sysconf(_SC_CLK_TCK);
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
  for (auto util : cpuUtils)
  {
    jiffies += (long)stof(util);
  }
  return jiffies;
}

// DONE: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  string filename = kProcDirectory + to_string(pid) + kStatFilename;
  vector<string> fields = getLineVectorFromFile(filename, 0);
  long utime = (long) stof(fields[14-1]);
  long stime = (long) stof(fields[15-1]);
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
  jiffies += stof(cpuUtils[4]);
  jiffies += stof(cpuUtils[5]);
  return jiffies;
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string filename = kProcDirectory + kStatFilename;
  return getLineVectorFromFile(filename, 0);
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string filename = kProcDirectory + kMeminfoFilename;
  string processes = getValueFromFile(filename, "processes");
  return (int)stof(processes);
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string filename = kProcDirectory + kMeminfoFilename;
  string processes = getValueFromFile(filename, "procs_running");
  return (int)stof(processes);
}

// DONE: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string filename = kProcDirectory + to_string(pid) + kCmdlineFilename;
  return getLineFromFile(filename, 0);
}

// DONE: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) { 
  string filename = kProcDirectory + to_string(pid) + kStatusFilename;
  return getValueFromFile(filename, "VmSize");
}

// DONE: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string filename = kProcDirectory + to_string(pid) + kCmdlineFilename;
  return getValueFromFile(filename, "Uid");
}

// DONE: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string uid = Uid(pid);
  return getValueFromFile(kPasswordPath, uid, 2, 0);
}

// DONE: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  string filename = kProcDirectory + to_string(pid) + kStatFilename;
  string upTime = getValueFromFile(filename, 22 - 1);
  float upTimeFloat = stof(upTime);
  return upTimeFloat / sysconf(_SC_CLK_TCK);
}

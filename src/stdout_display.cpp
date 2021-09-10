#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#include "stdout_display.h"
#include "system.h"
#include <linux_parser.h>

using std::cout;
using std::endl;
using std::string;
using std::to_string;

void StdOutDisplay::DisplaySystem(System& system) {
  cout << "Kernel            " << system.Kernel() << "\n";
  cout << "OperatingSystem   " << system.OperatingSystem() << "\n";
  cout << "CpuUtilization    " << system.Cpu().Utilization() << " %\n";
  cout << "MemoryUtilization " << system.MemoryUtilization() << " %\n";
  
  cout << "UpTime            " << system.UpTime() << " seconds\n";
  cout << "\n";
  cout << "TotalProcesses   " << system.TotalProcesses() << "\n";
  cout << "RunningProcesses " << system.RunningProcesses() << "\n";
}

void StdOutDisplay::DisplayProcesses(std::vector<Process>& processes, int n) {
  std::sort(processes.begin(), processes.end());
  float total = 0;
  for (int i = 0; i < n && i < (int)processes.size(); ++i) {
    Process &proc = processes[i];
    cout << proc.Pid() << "\t";
    cout << proc.UpTime() << "s\t"; 
    cout << proc.CpuUtilization() << "%\t";
    cout << proc.Ram() << "MB\t";
    cout << proc.Command() << "s\n"; 
  }
  for (auto proc : processes)
  {
    total += proc.CpuUtilization();
  }
  cout << "TOTAL!" << total << endl;
}

void StdOutDisplay::Display(System& system, int n) {
  DisplaySystem(system);
  DisplayProcesses(system.Processes(), n);
}

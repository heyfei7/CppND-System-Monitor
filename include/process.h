#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  int Pid() const;                               // DONE: See src/process.cpp
  std::string User() const;                      // DONE: See src/process.cpp
  std::string Command() const;                   // DONE: See src/process.cpp
  float CpuUtilization() const;                  // DONE: See src/process.cpp
  std::string Ram() const;                       // DONE: See src/process.cpp
  long int UpTime() const;                       // DONE: See src/process.cpp
  bool operator<(Process const& a) const;  // DONE: See src/process.cpp

  Process(int pid);
  // DONE: Declare any necessary private members
 private:
  int pid;
  std::string user;
  std::string cmd;
};

#endif
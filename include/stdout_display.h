#ifndef STDOUT_DISPLAY_H
#define STDOUT_DISPLAY_H

#include "process.h"
#include "system.h"

namespace StdOutDisplay {
void Display(System& system, int n = 10);
void DisplaySystem(System& system);
void DisplayProcesses(std::vector<Process>& processes, int n);
};  // namespace StdOutDisplay

#endif
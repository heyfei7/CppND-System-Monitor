#include "ncurses_display.h"
#include "stdout_display.h"
#include "system.h"

int main() {
  System system;
  //StdOutDisplay::Display(system);
  NCursesDisplay::Display(system);
}
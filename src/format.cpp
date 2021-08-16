#include <string>

#include "format.h"

using std::string;
using std::to_string;
// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
    int hh = seconds / 3600;
    int mm = (seconds % 3600) / 60;
    int ss = (seconds % 60);
    string HH = to_string(hh);
    string MM = to_string(mm);
    string SS = to_string(ss);
    if (HH.size() == 1)
    {
        HH = "0" + HH;
    }
    if (MM.size() == 1)
    {
        MM = "0" + MM;
    }
    if (SS.size() == 1)
    {
        SS = "0" + SS;
    }
    return HH + ":" + MM + ":" + SS;
}
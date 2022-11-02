#include <unistd.h>
#include "stdlib.h"
#include "stdio.h"
#include "stdlib.h"
#include <algorithm>
#include <string>
#include "Helper.hpp"
#include "ThreadManager.hpp"
#include <iostream>
#include <set>



using namespace BotManager;
using namespace std;

string BotManager::capitalizeString(string s)
{
    transform(s.begin(), s.end(), s.begin(), [](unsigned char c)
              { return toupper(c); });
    return s;
}

// custom sleep function
// int MY_THREADS::sleep_(long seconds)
// {
//     long milisec = seconds * 1000;
//     struct timespec rem;
//     struct timespec req = {
//         (long)(milisec / 1000), 
//         (milisec % 1000) * 1000000};
//     return nanosleep(&req, &rem);
// }

void BotManager::printf_debug(string s)
{
     try
        {
            std::cerr << s << endl;
            // cout.flush();
        }
        catch (const std::exception &e)
        {
            // std::cerr << e.what() << '\n';
        }
    
}

std::string BotManager::escape(const char* src, const set<char> escapee, const char marker)
{
  std::string r;
  while (char c = *src++)
  {
    if (escapee.find(c) != escapee.end())
      r += marker;
    r += c; // to get the desired behavior, replace this line with: r += c == '\n' ? 'n' : c;
  }
  return r;
}
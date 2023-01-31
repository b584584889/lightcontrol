#ifndef __LOG_H__
#define __LOG_H__

#include <string>
#include <fstream>

#define MODE_FILE 0
#define MODE_PRINTF 1

namespace vilslog
{
    extern const std::string path;
    extern std::ofstream out;
    void flush();
    int printf(const char* format, ...);
    int printRaw(const char* format, ...);
    void setMode(int mode); // 0:to file, 1:to printf
}

#endif //__LOG_H__

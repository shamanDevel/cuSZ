#include "format.hh"

#if _WIN32
//windows port of Linux-only functions
#include <memory.h>
#include <Windows.h>

int vasprintf(char** strp, const char* fmt, va_list ap)
{
    // _vscprintf tells you how big the buffer needs to be
    int len = _vscprintf(fmt, ap);
    if (len == -1) { return -1; }
    size_t size = (size_t)len + 1;
    char*  str  = static_cast<char*>(malloc(size));
    if (!str) { return -1; }
    // _vsprintf_s is the "secure" version of vsprintf
    int r = vsprintf_s(str, len + 1, fmt, ap);
    if (r == -1) {
        free(str);
        return -1;
    }
    *strp = str;
    return r;
}

#endif

#ifndef UTILS_FORMAT_HH
#define UTILS_FORMAT_HH

/**
 * @file format.hh
 * @author Jiannan Tian
 * @brief Formatting for log print (header).
 * @version 0.2
 * @date 2020-09-20
 * Created on 2020-04-27
 *
 * @copyright (C) 2020 by Washington State University, The University of Alabama, Argonne National Laboratory
 * See LICENSE in top-level directory
 *
 */

#include <iostream>
#include <sstream>
#include <string>

using std::string;

const string LOG_NULL      = "      ";
const string LOG_INFO      = "  ::  ";
const string LOG_ERR       = " ERR  ";
const string LOG_WARN      = "WARN  ";
const string LOG_DBG       = " dbg  ";
const string LOG_EXCEPTION = "  !!  ";

#if _WIN32
//windows port of Linux-only functions
#include <stdarg.h>

int vasprintf(char** strp, const char* fmt, va_list ap);
static inline int asprintf(char** strp, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int r = vasprintf(strp, fmt, ap);
    va_end(ap);
    return r;
}

#endif

// https://stackoverflow.com/a/26080768/8740097  CC BY-SA 3.0
template <typename T>
void build(std::ostream& o, T t)
{
    o << t << " ";
}

template <typename T, typename... Args>
void build(std::ostream& o, T t, Args... args)  // recursive variadic function
{
    build(o, t);
    build(o, args...);
}

template <typename... Args>
void LOGGING(const string& log_head, Args... args)
{
    std::ostringstream oss;
    oss << log_head;
    build(oss, args...);

    oss.seekp(0, std::ios::end);
    std::stringstream::pos_type offset = oss.tellp();
    if (log_head == LOG_DBG) { std::cout << "\e[2m"; }  // dbg
    std::cout << oss.str() << std::endl;                // print content
    if (log_head == LOG_DBG) std::cout << "\e[0m";      // finish printing dbg
}

#endif  // FORMAT_HH

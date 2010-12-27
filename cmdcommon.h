/*************************************************************************************************
 * Common symbols for command line utilities
 *                                                               Copyright (C) 2009-2010 FAL Labs
 * This file is part of Kyoto Tycoon.
 * This program is free software: you can redistribute it and/or modify it under the terms of
 * the GNU General Public License as published by the Free Software Foundation, either version
 * 3 of the License, or any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************************************/

#ifndef _CMDCOMMON_H                     // duplication check
#define _CMDCOMMON_H

#include <ktcommon.h>
#include <ktutil.h>
#include <ktsocket.h>
#include <ktthserv.h>

namespace kc = kyotocabinet;
namespace kt = kyototycoon;


// constants
const int32_t THREADMAX = 64;            // maximum number of threads
const size_t RECBUFSIZ = 64;             // buffer size for a record
const size_t RECBUFSIZL = 1024;          // buffer size for a long record
const size_t LINEBUFSIZ = 8192;          // buffer size for a line
const int32_t DEFPORT = 1978;            // default port number


// global variables
uint64_t g_rnd_x = 123456789;
uint64_t g_rnd_y = 362436069;
uint64_t g_rnd_z = 521288629;
uint64_t g_rnd_w = 88675123;


// function prototypes
void mysrand(int64_t seed);
int64_t myrand(int64_t range);
int64_t memusage();
void iprintf(const char* format, ...);
void iputchar(char c);
void eprintf(const char* format, ...);
void printversion();
void printdata(const char* buf, int size, bool px);
bool getline(std::istream* is, std::string* str);
void splitstr(const std::string& str, char delim, std::vector<std::string>* elems);
std::string unitnumstr(int64_t num);
std::string unitnumstrbyte(int64_t num);


// get the random seed
inline void mysrand(int64_t seed) {
  g_rnd_x = seed;
  for (int32_t i = 0; i < 16; i++) {
    myrand(1);
  }
}


// get a random number
inline int64_t myrand(int64_t range) {
  uint64_t t = g_rnd_x ^ (g_rnd_x << 11);
  g_rnd_x = g_rnd_y;
  g_rnd_y = g_rnd_z;
  g_rnd_z = g_rnd_w;
  g_rnd_w = (g_rnd_w ^ (g_rnd_w >> 19)) ^ (t ^ (t >> 8));
  return (g_rnd_w & INT64_MAX) % range;
}


// get the current memory usage
inline int64_t memusage() {
  std::map<std::string, std::string> info;
  kc::getsysinfo(&info);
  return kc::atoi(info["mem_rss"].c_str());
}


// print formatted information string and flush the buffer
inline void iprintf(const char* format, ...) {
  std::string msg;
  va_list ap;
  va_start(ap, format);
  kc::vstrprintf(&msg, format, ap);
  va_end(ap);
  std::cout << msg;
  std::cout.flush();
}


// print a character and flush the buffer
inline void iputchar(char c) {
  std::cout << c;
  std::cout.flush();
}


// print formatted error string and flush the buffer
inline void eprintf(const char* format, ...) {
  std::string msg;
  va_list ap;
  va_start(ap, format);
  kc::vstrprintf(&msg, format, ap);
  va_end(ap);
  std::cerr << msg;
  std::cerr.flush();
}


// print the versin information
inline void printversion() {
  iprintf("Kyoto Tycoon %s (%d.%d) on %s (Kyoto Cabinet %s)\n",
          kt::VERSION, kt::LIBVER, kt::LIBREV, kc::SYSNAME, kc::VERSION);
}


// print record data
inline void printdata(const char* buf, int size, bool px) {
  size_t cnt = 0;
  char numbuf[kc::NUMBUFSIZ];
  while (size-- > 0) {
    if (px) {
      if (cnt++ > 0) putchar(' ');
      std::sprintf(numbuf, "%02X", *(unsigned char*)buf);
      std::cout << numbuf;
    } else {
      std::cout << *buf;
    }
    buf++;
  }
}


// read a line from a file descriptor
inline bool getline(std::istream* is, std::string* str) {
  str->clear();
  bool hit = false;
  char c;
  while (is->get(c)) {
    hit = true;
    if (c == '\0' || c == '\r') continue;
    if (c == '\n') break;
    str->append(1, c);
  }
  return hit;
}


// convert a number into the string with the decimal unit
inline std::string unitnumstr(int64_t num) {
  if (num >= pow(1000.0, 6)) {
    return kc::strprintf("%.3Lf quintillion", (long double)num / pow(1000.0, 6));
  } else if (num >= pow(1000.0, 5)) {
    return kc::strprintf("%.3Lf quadrillion", (long double)num / pow(1000.0, 5));
  } else if (num >= pow(1000.0, 4)) {
    return kc::strprintf("%.3Lf trillion", (long double)num / pow(1000.0, 4));
  } else if (num >= pow(1000.0, 3)) {
    return kc::strprintf("%.3Lf billion", (long double)num / pow(1000.0, 3));
  } else if (num >= pow(1000.0, 2)) {
    return kc::strprintf("%.3Lf million", (long double)num / pow(1000.0, 2));
  } else if (num >= pow(1000.0, 1)) {
    return kc::strprintf("%.3Lf thousand", (long double)num / pow(1000.0, 1));
  }
  return kc::strprintf("%lld", (long long)num);
}


// convert a number into the string with the byte unit
inline std::string unitnumstrbyte(int64_t num) {
  if ((unsigned long long)num >= 1ULL << 60) {
    return kc::strprintf("%.3Lf EiB", (long double)num / (1ULL << 60));
  } else if ((unsigned long long)num >= 1ULL << 50) {
    return kc::strprintf("%.3Lf PiB", (long double)num / (1ULL << 50));
  } else if ((unsigned long long)num >= 1ULL << 40) {
    return kc::strprintf("%.3Lf TiB", (long double)num / (1ULL << 40));
  } else if ((unsigned long long)num >= 1ULL << 30) {
    return kc::strprintf("%.3Lf GiB", (long double)num / (1ULL << 30));
  } else if ((unsigned long long)num >= 1ULL << 20) {
    return kc::strprintf("%.3Lf MiB", (long double)num / (1ULL << 20));
  } else if ((unsigned long long)num >= 1ULL << 10) {
    return kc::strprintf("%.3Lf KiB", (long double)num / (1ULL << 10));
  }
  return kc::strprintf("%lld B", (long long)num);
}


// get the logger into the standard stream
inline kt::ThreadedServer::Logger* stdlogger(const char* prefix, std::ostream* strm) {
  class LoggerImpl : public kt::ThreadedServer::Logger {
  public:
    explicit LoggerImpl(std::ostream* strm, const char* prefix) :
      strm_(strm), prefix_(prefix) {}
    void log(Kind kind, const char* message) {
      const char* kstr = "MISC";
      switch (kind) {
        case kt::ThreadedServer::Logger::DEBUG: kstr = "DEBUG"; break;
        case kt::ThreadedServer::Logger::INFO: kstr = "INFO"; break;
        case kt::ThreadedServer::Logger::SYSTEM: kstr = "SYSTEM"; break;
        case kt::ThreadedServer::Logger::ERROR: kstr = "ERROR"; break;
      }
      *strm_ << prefix_ << ": [" << kstr << "]: " << message << std::endl;
    }
  private:
    std::ostream* strm_;
    const char* prefix_;
  };
  static LoggerImpl logger(strm, prefix);
  return &logger;
}


#endif                                   // duplication check

// END OF FILE

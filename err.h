#ifndef ERR_H
#define ERR_H

[[noreturn]] void syserr(const char* fmt, ...);
[[noreturn]] void fatal(const char* fmt, ...);

#endif

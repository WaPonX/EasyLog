#ifndef LOG_H
#define LOG_H

#include "stacktrace.h"

namespace waponxie {
namespace log{
void log(FILE *out, const char *file, const char *func, int line) {
   fprintf(out, "in %s %d function: %s\n", file, line, func);
   print_stacktrace(out);
}

}

#ifndef log
#define log() waponxie::log::log(stderr, __FILE__, __func__, __LINE__)
#endif
}

#endif // LOG_H

#ifndef _STACKTRACE_H_
#define _STACKTRACE_H_

#include <assert.h>
#include <cxxabi.h>
#include <execinfo.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef LOG_MAX_TRACE 
#define LOG_MAX_TRACE 64
#endif

namespace waponxie {
namespace log{
/** Print a demangled stack backtrace of the caller function to FILE* out. */
void print_stacktrace(FILE *out = stderr) {
    assert(LOG_MAX_TRACE > 0);
    fprintf(out, "Traceback:\n");

    // because gcc will add extra 2 message.
    void* addressList[LOG_MAX_TRACE + 2];

    int addressListCount = backtrace(addressList, LOG_MAX_TRACE + 2);

    if(0 == addressListCount) {
        fprintf(out, "empty stack trace.\n");
        return;
    }

    // resolve symbol into strings containing "filename(function+address)",
    // funtion backtrace_symbols will retuan a memory if it malloc memory successfully.
    char** symbolList = backtrace_symbols(addressList, addressListCount);

    // allocate string which will be filled with the demangled function name
    size_t funcNameSize = 512;
    char* funcName = (char*)malloc(funcNameSize);

    // iterate over the returned symbol list.
    // ignore the first, it is the symbol of this function.
    for (int i = addressListCount - 1; i > 0; --i) {
        char *beginName = NULL;
        char *beginOffset = NULL;
        char *endOffset = NULL;

        // find parentheses and +address offset surrounding the mangled name:
        // ./module(function+0x15c) [0x8048a6d]
        for (char *p = symbolList[i]; *p; ++p) {
            if (*p == '(')  beginName = p;
            else if (*p == '+') beginOffset = p;
            else if (*p == ')' && beginOffset) {
                endOffset = p; 
                break;
            }
        }

        if (beginName && beginOffset && endOffset
                && beginName < beginOffset) {
            *beginName++ = '\0';
            *beginOffset++ = '\0';
            *endOffset = '\0';

            // mangled name is now in [beginName, beginOffset) and caller
            // offset in [beginOffset, endOffset). now apply
            // __cxa_demangle():

            int status;
            char* ret = abi::__cxa_demangle(beginName,
                    funcName, &funcNameSize, &status);
            if (status == 0) {
                funcName = ret; // use possibly realloc()-ed string
                fprintf(out, "  %s : %s+%s\n",
                        symbolList[i], funcName, beginOffset);
            }
            else {
                // demangling failed. Output function name as a C function with
                // no arguments.
                fprintf(out, "  %s : %s()+%s\n",
                        symbolList[i], beginName, beginOffset);
            }
        }
        else {
            // couldn't parse the line? print the whole line.
            fprintf(out, "  %s\n", symbolList[i]);
        }
    }

    free(funcName);
    free(symbolList);
}
} // namespace log
} // namespace waponxie

#endif // _STACKTRACE_H_

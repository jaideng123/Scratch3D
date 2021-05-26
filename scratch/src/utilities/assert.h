#pragma once

#include <iostream>

#ifdef _MSC_VER
#define DEBUG_BREAK() __debugbreak()
#else
#define DEBUG_BREAK() asm {int 3}
#endif

#ifdef DEBUG

inline void reportAssertionFailure(std::string expression, std::string filename, int lineNumber) {
    std::cerr << "ASSERTION FAILED AT " << filename << ":" << lineNumber << " (" << expression << ")" << std::endl;
}

#define SCRATCH_ASSERT_NEVER(message) \
    reportAssertionFailure(message, __FILE__, __LINE__); \
    DEBUG_BREAK();\

#define SCRATCH_ASSERT(expr) \
    if(expr) {}      \
    else{            \
        reportAssertionFailure(#expr, __FILE__, __LINE__); \
        DEBUG_BREAK();\
    }

#else
#define SCRATCH_ASSERT(expr)
#define SCRATCH_ASSERT_NEVER(message)


#endif
#pragma once

#if defined(_WIN32) || defined(_WIN64)
    #if defined(TESTDOMAIN_STATIC)
        #define TESTDOMAIN_API
    #elif defined(TESTDOMAIN_EXPORTS)
        #define TESTDOMAIN_API __declspec(dllexport)
    #else
        #define TESTDOMAIN_API __declspec(dllimport)
    #endif
#else
    #define TESTDOMAIN_API __attribute__((visibility("default")))
#endif
#pragma once

#if defined(_WIN32) || defined(_WIN64)
    #ifdef BASELIB_EXPORTS
        #define BASELIB_API __declspec(dllexport)
    #else
        #define BASELIB_API __declspec(dllimport)
    #endif
#else
    #define BASELIB_API __attribute__((visibility("default")))
#endif

#pragma once

#if defined(_WIN32) || defined(_WIN64)
    #ifdef BABELWIRESLIB_EXPORTS
        #define BABELWIRESLIB_API __declspec(dllexport)
    #else
        #define BABELWIRESLIB_API __declspec(dllimport)
    #endif
#else
    #define BABELWIRESLIB_API __attribute__((visibility("default")))
#endif

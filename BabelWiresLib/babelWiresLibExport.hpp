#pragma once

#if defined(_WIN32) || defined(_WIN64)
    #if defined(BABELWIRESLIB_STATIC)
        #define BABELWIRESLIB_API
        #define BABELWIRESLIB_EXTERNING_INSTANTIATION 
    #elif defined(BABELWIRESLIB_EXPORTS)
        #define BABELWIRESLIB_API __declspec(dllexport)
        #define BABELWIRESLIB_EXTERNING_INSTANTIATION 
    #else
        #define BABELWIRESLIB_API __declspec(dllimport)
        #define BABELWIRESLIB_EXTERNING_INSTANTIATION __declspec(dllimport)
    #endif
#else
    #define BABELWIRESLIB_API __attribute__((visibility("default")))
    #define BABELWIRESLIB_EXTERNING_INSTANTIATION BABELWIRESLIB_API
#endif

#pragma once

#if defined(_WIN32) || defined(_WIN64)
    #if defined(BABELWIRESQTUI_STATIC)
        #define BABELWIRESQTUI_API
    #elif defined(BABELWIRESQTUI_EXPORTS)
        #define BABELWIRESQTUI_API __declspec(dllexport)
    #else
        #define BABELWIRESQTUI_API __declspec(dllimport)
    #endif
#else
    #define BABELWIRESQTUI_API __attribute__((visibility("default")))
#endif

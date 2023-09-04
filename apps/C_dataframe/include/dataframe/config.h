#ifndef GUARD_EBBBA18C_A57A_41C4_8369_0278C7C21695
#define GUARD_EBBBA18C_A57A_41C4_8369_0278C7C21695

#if !defined(DATAFRAME_STATIC) && defined(_WIN32)
    #if defined(DATAFRAME_COMPILE_DLL)
        #define DATAFRAME_EXPORT __declspec(dllexport)
    #else
        #define DATAFRAME_EXPORT __declspec(dllexport)
    #endif
#else
    #define DATAFRAME_EXPORT
#endif

#include <stdbool.h>

#endif

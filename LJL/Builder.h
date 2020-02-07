#pragma once
#include <cstdint> // uint_least8_t

#include "Global.h" // Global defines

LJL_BEGIN
    // Function building blocks
    namespace Builder {
        // __cdecl epilogs and prologs
        const uint_least8_t prolog[] = { 
            0x55,                   // push %rbp
            0x48, 0x89, 0xe5        // mov %rbp, %rsp
        };
        const uint_least8_t epilog[] = { 
            0x5d,                   // pop %rbp
            0xc3                    // ret
        };
    }
LJL_END
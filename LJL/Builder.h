#pragma once
#include <cstdint> // uint_least8_t

#include "Global.h" // Global defines

LJL_BEGIN
    // Function building blocks
    namespace Builder {
        // __cdecl epilogs and prologs
        const uint_least8_t epilog[] = { 
            0x55,                   // push %rbp
            0x48, 0x89, 0xe5,       // mov %rbp, %rsp
            0x48, 0x8b, 0x45, 0x08, // mov %rax, QWORD PTR [%rbp+0x8]
            0x48, 0x8b, 0x55, 0x0c  // mov %rax, QWORD PTR [%rbp+0xc]
        };
        const uint_least8_t prolog[] = { 
            0x5d,                   // pop %rbp
            0xc3                    // ret
        };
    }
LJL_END
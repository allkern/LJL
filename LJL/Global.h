#pragma once

// Version defines
#define LJL_MAJOR "0"
#define LJL_MINOR "1"
#define LJL_REL "r1"

// Namespace begin and end markers
#define LJL_BEGIN namespace ljl {
#define LJL_END }

// Calling convention used (__cdecl only as of now)
#define LJL_DECL __cdecl
#define LJL_DECL_C

// Type aliases for commonly used types
#define LJL_VOID void
#define LJL_BOOL bool

// Architecture description defines (not yet implemented)
#define LJL_ARCH_ORDER_SMALL_ENDIAN
#define LJL_ARCH_X64

#pragma once

// Version defines
#define LJL_MAJOR "0"
#define LJL_MINOR "2"
#define LJL_REL "r2"

// Namespace begin and end markers
#define LJL_BEGIN namespace ljl {
#define LJL_END }

// Calling convention used (__cdecl only as of now)
#define LJL_DECL __cdecl
#define LJL_DECL_C

// Type aliases for commonly used types
#define LJL_VOID void
#define LJL_BOOL bool
#define LJL_SIZE size_t

// Architecture description defines (unimplemented)
#ifdef LJL_X64
	#define LJL_ARCH_X64
	#define LJL_ARCH_ORDER_SMALL_ENDIAN
#endif

#ifdef LJL_X86
	#define LJL_ARCH_X86
	#define LJL_ARCH_ORDER_SMALL_ENDIAN
#endif

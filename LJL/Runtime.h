#pragma once
#include <iostream>
#include <csignal>
#include <cstdlib>

#include "Global.h"

LJL_BEGIN


class Runtime { // Unimplemented
	virtual inline void __cdecl sigsegv() {
		std::cout << "LJLRT error: SIGSEGV was raised.\n"
				  << "Possible causes include:\n"
				  << "- Bad stack alignment/contents (wrong return addresses, etc)\n"
				  << "- Uninitialized register used as memory operand\n";
		std::abort();
	};
};

LJL_END
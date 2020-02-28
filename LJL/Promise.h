#pragma once

#include "Global.h"

LJL_BEGIN

// ljl::Promise:
// This class allows for dynamic patching of code.
template <class T> struct Promise {
	size_t source_offset;

	Promise() = default;
	Promise(size_t source_offset) : source_offset(source_offset) {}
    size_t get_source_offset() { return source_offset; }
};

LJL_END
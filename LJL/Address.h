#pragma once

#include "Global.h"

#include <cstdint>	// uint_least64_t

LJL_BEGIN
	// This class represents a memory address, to be used as an operand.
	template <class T> class Address {
		// Pointer to an object
		T* ptr;
	public:
		// Construct directly from an object (passed by reference)
		Address(T& obj) : ptr(&obj) {}
		
		// Construct from a pointer
		Address(T* ptr) : ptr(ptr) {}

		// Default constructor
		Address() = default;

		// Cast to uint_least64_t
		inline uint_least64_t getLiteral() { return (uint_least64_t)this->ptr; }

		// Same as constructors
		inline void init(T& obj) { this->ptr = &obj; }
		inline void init(T* obj) { this->ptr = obj; }
	};
LJL_END
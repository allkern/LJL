#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <queue>

#include "Function.h"
#include "Aliases.h"
#include "Address.h"
#include "Global.h"

LJL_BEGIN

// ljl::Bind:
// This class facilitates the usage of 64-bit absolute addresses within a program.
// Contains information about a target address

struct Bind {
	// Symbols make searching binds easier
	std::string symbol;
	
	// The actual 64-bit absolute address
	u64 target_address;

	// Constructors
	Bind() = default;
	Bind(u64 target_address, std::string symbol) :
		symbol(symbol),
		target_address(target_address) {}
};

// ljl::BindRequest:
// Bind requests are like promises, they represent where a binding should be placed on the compiled code.
struct BindRequest {
	std::string symbol;

	// Where the bind should be placed
	size_t source_offset;
public:
	// Constructor
	BindRequest(std::string symbol, size_t source_offset) :
		symbol(symbol),
		source_offset(source_offset) {}
};

// ljl::BindTable:
// BindTable is a class that facilitates bind searching and organization
class BindTable {
	// Unknown symbol counter
	size_t usn = 0;

	// These two vectors keep track of binds, and bind requests
	std::vector <Bind> binds;
	std::vector <BindRequest> bindRequests;

	// Formats symbols to this form: "<symbol_name>", or in the case of an unknown symbol: "<_Bind#_>"
	void formatSymbol(std::string& symbol) {
		if (symbol == "<unknown>") {
			symbol = "_Bind" + std::to_string(usn) + "_";
		} ++usn;
		symbol = "<" + symbol + ">";
	}

	// These methods return pointers to the binds, and requests vectors, respectively
	std::vector <Bind>* getBinds() { return &binds; }
	std::vector <BindRequest>* getRequests() { return &bindRequests; }

	// Get the real bind table size (as in after generation)
	size_t getBindTableSize() { return binds.size()*8; }

	// Get the index of a bind by its symbol
	size_t getIndexBySymbol(std::string symbol) {
		size_t idx = 0;
		for (Bind b : binds) {
			if (b.symbol == symbol) { return idx; }
			++idx;
		}
	}

	// Allows ljl::Function<T> to build bind tables
	template <class T> friend class Function;

public:
	// Request a binding
	LJL_VOID request(BindRequest r) {
		bindRequests.push_back(r);
	}

	// Create a binding
	LJL_VOID push(Bind& bind) {
		formatSymbol(bind.symbol);
		binds.push_back(bind);
	}
};

LJL_END
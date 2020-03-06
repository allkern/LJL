#pragma once

#include <iostream>
#include <vector>
#include <string>

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
	std::vector <BindRequest> bind_requests;

	// Formats symbols to this form: "<symbol_name>", or in the case of an unknown symbol: "<_Bind#_>"
	void format_symbol(std::string& symbol) {
		if (symbol == "<unknown>") {
			symbol = "_Bind" + std::to_string(usn) + "_";
		} ++usn;
		symbol = "<" + symbol + ">";
	}

	// These methods return pointers to the binds, and request vectors, respectively
	std::vector <Bind>* get_binds() { return &binds; }
	std::vector <BindRequest>* get_requests() { return &bind_requests; }

	// Get the real bind table size (as in after generation)
	size_t get_real_size() { return binds.size()*8; }
	size_t get_size() { return binds.size(); }

	// Get the index of a bind by its symbol
	size_t get_index_by_symbol(std::string symbol) {
		size_t idx = 0;
		for (Bind b : binds) {
			if (b.symbol == symbol) { return idx; }
			++idx;
		}
	}

	// Allows ljl::Function<T> to build bind tables
	template <class ReturnType, class... Args> friend class Builder;

public:
	// Request a binding
	LJL_VOID request(BindRequest r) {
		bind_requests.push_back(r);
	}

	// Create a binding
	LJL_VOID push(Bind& bind) {
		format_symbol(bind.symbol);
		binds.push_back(bind);
	}

	LJL_VOID push(Bind bind) {
		format_symbol(bind.symbol);
		binds.push_back(bind);
	}

	std::vector <ljl::u8> generate() {
		std::vector <ljl::u8> generated_table;
        size_t insert_counter = 0;
        for (Bind& bind : binds) {
            uint_least64_t m = 0;
		    size_t t = 0;
		    for (size_t b = 0; b < 8; b++) {
			    t = b*8;
			    m = ((uint_least64_t)0xff << t);
			    generated_table.insert(
					generated_table.begin() + insert_counter++, 
					(ljl::u8)((bind.target_address&m)>>t)
				);
		    }
        }
		return generated_table;
    }
};

LJL_END
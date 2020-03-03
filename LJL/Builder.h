#pragma once
#include <initializer_list>
#include <iostream>
#include <iomanip>
#include <cstdint> // uint_least8_t

#include "Global.h" // Global defines
#include "Function.h"
#include "Aliases.h"
#include "Address.h" // ljl::Address<T>
#include "Promise.h"
#include "BindTable.h"

LJL_BEGIN

template <class ReturnType> class Builder {
    Function <ReturnType>* f = nullptr;
    BindTable* bind_table = nullptr;

    // __cdecl prologs and epilogs
    //#ifdef LJL_DECL_C
        static constexpr u8 prolog[] = {
            0x55,                   // push %rbp
            0x48, 0x89, 0xe5        // mov %rbp, %rsp
        };
        static constexpr u8 epilog[] = { 
            0x5d,                   // pop %rbp
            0xc3                    // ret
        };
    //#endif

    // Prolog, and epilog pushers
    void generate_prolog() { for (ljl::u8 o : prolog) { f->push_back(o); } }
    void generate_epilog() { for (ljl::u8 o : epilog) { f->push_back(o); } }

    template <class T> LJL_VOID overwrite(LJL_SIZE idx, T a) {
		T mask = 0; size_t shift = 0;
		for (size_t b = 0; b < sizeof(T); b++) {
			shift = b*8;
			mask  = ((T)0xff << shift);
			f->at(idx+b) = ((a&mask)>>shift);
		}
	}

    LJL_VOID link_binds() {
        auto bind_requests = bind_table->get_requests();
        size_t bind_table_size = bind_table->get_size(),
               bind_counter = 0;
        for (BindRequest& br : *bind_requests) {
            size_t code_offset = br.source_offset + bind_table_size*8,
                   bt_offset   = bind_table->get_index_by_symbol(br.symbol)*8;
            i32    rip_offset  = -(i32)((code_offset+4) - bt_offset);
            overwrite(code_offset, rip_offset);
        }
    }

public:
    LJL_VOID change_bind_table(BindTable* bt_new) { bind_table = bt_new; }
    
    LJL_SIZE get_expected_rip() { return f->size(); }

    // Initialize the function.
    LJL_VOID initialize() {
        generate_prolog();
    }

    Builder() = default;
    Builder(Function <ReturnType>& f) : f(&f) { initialize(); }
    Builder(Function <ReturnType>& f, BindTable& b) : f(&f), bind_table(&b) { initialize(); }

    // Append a literal
    template <class T> LJL_VOID append(T a) {
		T mask = 0; size_t shift = 0;
		for (size_t b = 0; b < sizeof(T); b++) {
			shift = b*8;
			mask  = ((T)0xff << shift);
			f->push_back((a&mask)>>shift);
		}
	}

    // Append an std::initializer_list
    LJL_VOID append(std::initializer_list <uint_least8_t> c) {
        f->insert(f->end(), c.begin(), c.end());
    }

    // Append a memory address
    template <class T> LJL_VOID append(Address<T> addr) {
		ljl::u64 a = addr.get_literal();
        append(a);
	}

    template <class T> Promise<T> promise() {
        Promise<T> p(get_expected_rip());
        append((T)0);
        return p;
    }

    template <class T> LJL_VOID fulfill(Promise<T>& p, T value) {
        overwrite(p.source_offset, value);
    }

    LJL_VOID link(std::string symbol = "<unknown>") {
        bind_table->request(BindRequest(symbol, get_expected_rip()));
        append((u32)0);
    }

    LJL_VOID make() {
        generate_epilog();
        if (bind_table != nullptr) {
            auto generated_table = bind_table->generate();
            f->insert(f->begin(), generated_table.begin(), generated_table.end());
            f->entry_offset = bind_table->get_real_size();
            link_binds();
        }
        f->make();
    }
};

LJL_END
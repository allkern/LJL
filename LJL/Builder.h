#pragma once
#ifdef LJL_SPEC_APPEND_INITL
    #include <initializer_list>
#endif

#ifdef LJL_SPEC_APPEND_STRING
    #include <string>
#endif

#include <iostream>
#include <iomanip>
#include <cstdint> // uint_least8_t

#include "Global.h" // Global defines
#include "Function.h"
#include "Aliases.h"

#ifdef LJL_SPEC_ADDRESSES
#include "Address.h" // ljl::Address<T>
#endif

#ifdef LJL_SPEC_PROMISES
#include "Promise.h"
#endif

#ifdef LJL_SPEC_BINDS
#include "BindTable.h"
#endif

LJL_BEGIN

template <class ReturnType, class... Args> class Builder {
    Function <ReturnType, Args...>* f = nullptr;

#ifdef LJL_SPEC_BINDS
    BindTable* bind_table = nullptr;
#endif

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

#ifdef LJL_SPEC_BINDS
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
    LJL_VOID switch_bind_table(BindTable* bt_new) { bind_table = bt_new; }

    LJL_VOID link(std::string symbol = "<unknown>") {
        bind_table->request(BindRequest(symbol, get_expected_rip()));
        append((u32)0);
    }
#endif

public:
    // Get the expected value of RIP at the current state of the buffer
    LJL_SIZE get_expected_rip() { return f->size(); }

    // Initialize the function.
    LJL_VOID initialize() {
#ifdef LJL_SPEC_AUTO_PROLOG
        generate_prolog();
#endif
    }

    Builder() = default;
    Builder(Function <ReturnType(Args...)>& f) : f(&f) { initialize(); }

#ifdef LJL_SPEC_BINDS
    Builder(Function <ReturnType(Args...)>& f, BindTable& b) : f(&f), bind_table(&b) { initialize(); }
#endif

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
#ifdef LJL_SPEC_APPEND_INITL
    LJL_VOID append(std::initializer_list <uint_least8_t> c) {
        f->insert(f->end(), c.begin(), c.end());
    }
#endif

    // Append a memory address
#ifdef LJL_SPEC_APPEND_ADDRESS
    template <class T> LJL_VOID append(Address<T> addr) {
		ljl::u64 a = addr.get_literal();
        append(a);
	}
#endif

#ifdef LJL_SPEC_APPEND_STRING
    LJL_VOID append(std::string c) {
        size_t p;
        while (c.size()) {
            append((ljl::u8)std::stoul("0x"+c[0]+c[1], nullptr));
            p += 2; c.substr(p);
        }
    }
#endif

#ifdef LJL_SPEC_PROMISES
    template <class T> Promise<T> promise() {
        Promise<T> p(get_expected_rip());
        append((T)0);
        return p;
    }

    template <class T> LJL_VOID fulfill(Promise<T>& p, T value) {
        overwrite(p.source_offset, value);
    }
#endif

    LJL_VOID make() {
#ifdef LJL_SPEC_AUTO_EPILOG
        generate_epilog();
#endif
#ifdef LJL_SPEC_BINDS
        if (bind_table != nullptr) {
            auto generated_table = bind_table->generate();
            f->insert(f->begin(), generated_table.begin(), generated_table.end());
            f->entry_offset = bind_table->get_real_size();
            link_binds();
        }
#endif
        f->make();
    }
};

LJL_END
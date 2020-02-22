#pragma once
#include <initializer_list> // std::initializer_list<T>
#include <type_traits>      // std::is_same<T1, T2>
#include <cstdint>          // uint_least8_t, uint_least64_t
#include <cassert>          // assert()
#include <vector>           // std::vector<T>
#include <iomanip>

#define WIN32_LEAN_AND_MEAN // Optimize size of "Windows.h"
#include "Windows.h" // VirtualProtect

#include "Global.h" // Global defines
#include "Address.h" // ljl::Address<T>
#include "Builder.h" // Builder::epilog, Builder::prolog
#include "BindTable.h" // ljl::BindTable, ljl::Bind

LJL_BEGIN
    // ljl::Promise:
    // This class allows for dynamic patching of code.
    template <class T> class Promise {
	    size_t source_offset;
    
    public:
	    Promise() = default;
	    Promise(size_t source_offset) : source_offset(source_offset) {}
        size_t getSourceOffset() { return source_offset; }
    template <class F> friend class Function;
    };

    // ljl::Function<T>:
    // This is ljl's main class, it serves as a code buffer, and code builder
    template <class _Rty> class Function : public std::vector <uint_least8_t> {
        // Allows bindings
        BindTable bindTable;

        size_t expectedRIP = 0;

        // This bool will be asserted on calls. It assures the object is executable before calling
        bool IS_EXECUTABLE = false;
        
        // Push the function epilog's code according to the calling convention defined
        void generateEpilog() {
            for (uint_least8_t o : Builder::epilog) {
                push_back(o);
                ++expectedRIP;
            }
        }

        // Push the function prolog's code according to the calling convention defined
        void generateProlog() {
            for (uint_least8_t o : Builder::prolog) {
                push_back(o);
                ++expectedRIP;
            }
        }

        LJL_VOID generateBindTable() {
            auto bindTablePtr = bindTable.getBinds();
            size_t bindTableInsert = 0;
            for (Bind& bind : *bindTablePtr) {
                uint_least64_t m = 0;
		        size_t t = 0;
		        for (size_t b = 0; b < 8; b++) {
			        t = b*8;
			        m = ((uint_least64_t)0xff << t);
			        insert(begin() + bindTableInsert++, (ljl::u8)((bind.target_address&m)>>t));
		        }
            }
        }

        LJL_VOID linkBinds() {
            auto bindReqsPtr = bindTable.getRequests();
            size_t bindTableSize = bindTable.getBindTableSize(),
                   bindCt = 0, bindTableInsert = 0;
            for (BindRequest& br : *bindReqsPtr) {
                size_t codeOffset = br.source_offset + bindTableSize,
                       btOffset   = bindTable.getIndexBySymbol(br.symbol)*8;
                i32    ripOffset  = -(i32)((codeOffset+4) - btOffset);
                for (size_t b = 0; b < 4; b++) {
			        size_t t = b*8;
			        uint_least64_t m = ((uint_least64_t)0xff << t);
			        at(codeOffset + bindTableInsert++) = (ljl::u8)((ripOffset&m)>>t);
		        } bindTableInsert = 0;
            }
        }

    public:
        // Default constructor
        Function() { init(); }

        // Initialize the function.
        LJL_VOID init() {
            generateProlog();
        }

        size_t getExpectedRIP() { return expectedRIP; }

        // Append an std::initializer_list
        LJL_VOID append(std::initializer_list <uint_least8_t> c) {
            insert(end(), c.begin(), c.end());
            expectedRIP += c.size();
        }

        // Append a memory address
        template <class T> LJL_VOID append(Address<T> addr) {
		    uint_least64_t a = addr.getLiteral();
            append(a);
	    }

        LJL_VOID bind(Bind& bind) {
            bindTable.push(bind);
        }

        LJL_VOID bind(Bind bind) {
            bindTable.push(bind);
        }

        template <class T> Promise<T> promise() {
            Promise<T> p(getExpectedRIP());
            append((T)0);
            return p;
        }

        template <class T> LJL_VOID fulfill(Promise<T>& p, T value) {
            for (size_t b = 0; b < sizeof(T); b++) {
			    size_t t = b*8;
			    uint_least64_t m = ((uint_least64_t)0xff << t);
			    at((p.source_offset)+b) = (ljl::u8)((value&m)>>t);
		    }
        }

        LJL_VOID link(std::string symbol = "<unknown>") {
            bindTable.request(BindRequest(symbol, getExpectedRIP()));
            append((u32)0);
        }

        // Append a literal
        template <class T> LJL_VOID append(T a) {
		    uint_least64_t m = 0;
		    size_t t = 0;
		    for (size_t b = 0; b < sizeof(T); b++) {
			    t = b*8;
			    m = ((uint_least64_t)0xff << t);
			    push_back((a&m)>>t);
		    }
            expectedRIP += sizeof(T);
	    }

        // Append a prolog, and make the function executable
        LJL_BOOL make() {
            generateEpilog();
            generateBindTable();
            linkBinds();
            DWORD old;
            IS_EXECUTABLE = (bool)VirtualProtect(
                data(),
                sizeof(this->data()),
                PAGE_EXECUTE_READWRITE,
                &old
            );
            return IS_EXECUTABLE;
        }

        // Invoke the function
        inline _Rty operator()() {
            // Guarantee function is executable before invoking.
            assert( IS_EXECUTABLE == true );
            // Check if function returns or is void
            auto funcPtr = &data()[bindTable.getBindTableSize()];
            if constexpr (std::is_same <_Rty, void>::value == true) {
                ((_Rty( LJL_DECL *)(void))funcPtr)();
            } else {
                return ((_Rty( LJL_DECL *)(void))funcPtr)();
            }
        }
    };

LJL_END
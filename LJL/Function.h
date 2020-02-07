#pragma once
#include <initializer_list> // std::initializer_list<T>
#include <type_traits>      // std::is_same<T1, T2>
#include <cstdint>          // uint_least8_t, uint_least64_t
#include <cassert>          // assert()
#include <vector>           // std::vector<T>

#define WIN32_LEAN_AND_MEAN // Optimize size of <Windows.h>
#include "Windows.h" // VirtualProtect

#include "Global.h" // Global defines
#include "Address.h" // ljl::Address<T>
#include "Builder.h" // Builder::epilog, Builder::prolog

LJL_BEGIN
    template <class _Rty> class Function : public std::vector <uint_least8_t> {
        // This bool will be asserted on calls. It assures the object is executable before calling
        bool IS_EXECUTABLE = false;
        
        // Push the function epilog's code according to the calling convention defined
        void generateEpilog() {
            for (uint_least8_t o : Builder::epilog) {
                push_back(o);
            }
        }

        // Push the function prolog's code according to the calling convention defined
        void generateProlog() {
            for (uint_least8_t o : Builder::prolog) {
                push_back(o);
            }
        }

    public:
        // Default constructor
        Function() { init(); }

        // Initialize the function.
        LJL_VOID init() {
            generateProlog();
        }

        // Append an std::initializer_list
        LJL_VOID append(std::initializer_list <uint_least8_t> c) {
            insert(end(), c.begin(), c.end());
        }

        // Append a memory address
        template <class T> LJL_VOID append(ljl::Address<T> addr) {
		    uint_least64_t a = addr.getLiteral();
            append(a);
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
	    }

        // Append a prolog, and make the function executable
        LJL_BOOL make() {
            generateEpilog();
        #ifdef WIN32
            DWORD old;
            IS_EXECUTABLE = (bool)VirtualProtect(
                data(),
                sizeof(this->data()),
                PAGE_EXECUTE_READWRITE,
                &old
            );
            return IS_EXECUTABLE;
        #endif
        }

        // Invoke the function
        inline _Rty operator()() {
            // Guarantee function is executable before invoking.
            assert( IS_EXECUTABLE == true );
            // Check if function returns or is void
            if constexpr (std::is_same <_Rty, void>::value == true) {
                ((_Rty( LJL_DECL *)(void))data())();
            } else {
                return ((_Rty( LJL_DECL *)(void))data())();
            }
        }
    };
LJL_END
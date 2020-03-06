#pragma once
#include <type_traits>      // std::is_same<T1, T2>
#include <iostream>
#include <vector>           // std::vector<T>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN // Optimize size of "Windows.h"
    #include "Windows.h" // VirtualProtect
#endif

#include "Global.h" // Global defines

LJL_BEGIN

    // ljl::Function<T, Args...>:
    // A code buffer, manageable through ljl::Builder.
    template <class ReturnType, class... Args> class Function : public std::vector <ljl::u8> {
        // This bool will be asserted on calls. It assures the object is executable before calling
        bool IS_EXECUTABLE = false;

        // Append a prolog, and make the function executable
        LJL_BOOL make() {
            DWORD old;
            IS_EXECUTABLE = (bool)VirtualProtect(
                data(),
                sizeof(this->data()),
                PAGE_EXECUTE_READWRITE,
                &old
            );
            return IS_EXECUTABLE;
        }

    size_t entry_offset = 0;
    template <class ReturnType, class... Args> friend class Builder;
    
    public:
        // Default constructor
        Function() = default;

        // Invoke the function
        inline ReturnType operator()() {
            // Guarantee function is executable before invoking.
            // Check if function returns or is void
            auto funcPtr = &data()[entry_offset];
            if constexpr (std::is_same <ReturnType, void>::value == true) {
                ((ReturnType( LJL_DECL *)(void))funcPtr)(Args... args);
            } else {
                return ((ReturnType( LJL_DECL *)(void))funcPtr)(Args... args);
            }
        }
    };

LJL_END
![LJL's banner](https://user-images.githubusercontent.com/15825466/73777526-62530400-4768-11ea-80b6-92d8a13ebff1.png)

# LJL (Lycoder's JIT Library)
LJL is a simple x86-64 JIT function generation library written in C++17 that is compatible with STL containters, `std::thread`, etc.
As of now, mainly intended to be used on emulators, or as a learning tool, either of JIT code generation, or Assembly.
Check the wiki for tutorials, and further information about LJL!

## Short/medium term goals
 - Create an "extensions" system that allows developers to choose whether some feature is included in a project or not, making performance-critical, or low-resource development easier (work being done in the development branch)
 - Make debugging easier by creating assemblers, disassemblers, and a runtime (as extensions)

## Long term goals
 - Become **the** code generation library of choice in at least one of the following fields:
   * Performance-critical/low-resource development
   * JIT Compilers
   * Emulators
   * Teaching assembly/JIT code generation


## Simple example (Windows)
```c++
#include <iostream>
#include <array>

#include "LJL\ljl.h"

int main() {
	// Create a function, and builder with return type <void>
	ljl::Function <void> myFunction;
	ljl::Builder myBuilder(myFunction);
	
	// Create an STL array
	std::array <int, 10> myArray = { 0 };

	// Create an address from a pointer to an array
	ljl::Address myAddress(myArray.data());
  	
        // Code:
        // movabs rax         , myAddress
        // mov    rcx         , 0x1
        // mov    [rax+rcx*4] , 0xcafe
  
	myBuilder.append( {
		0x48, 0xb8                    
	} );

	myBuilder.append(myAddress);

	myBuilder.append( {
		0x48, 0xc7, 0xc1
	} );

	myBuilder.append((ljl::u32)0x1);
  
  
	myBuilder.append( {
		0x48, 0xC7, 0x04, 0x88
	} );

	myBuilder.append((ljl::u32)0xcafe);

	// Append prolog and make the page executable
	myBuilder.make();

	// Call the generated function
	myFunction();

	// Print the array
	size_t p = 0;
	for (int a : myArray) {
		std::cout << "myArray[" << p++ << "] = " << std::hex << a << std::endl;
	}
	system("pause");
	return 0;
}
```

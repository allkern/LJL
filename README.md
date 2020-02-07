<div style="text-align:center"><img src="https://user-images.githubusercontent.com/15825466/73777526-62530400-4768-11ea-80b6-92d8a13ebff1.png" width="1900" height="154" alt="Photo of 3 cats"/></div>


# LJL (Lycoder's Jit Library)
LJL is a simple x86-64 JIT function generation library written in C++17 that is compatible with STL containters, `std::thread`, etc.
Mainly intended to be used on emulators, or as a learning tool, either of JIT code generation, or Assembly.
Check the wiki for tutorials, and further information about LJL!


## Simple example (Windows)
```c++
#include <iostream>
#include <array>

#include "LJL\ljl.h"

int main() {
	// Create a function with return type <void>
	ljl::Function <void> myFunction;

	// Create an STL array
	std::array <int, 10> myArray = { 0 };

	// Create an address from a pointer to an array
	ljl::Address myAddress(myArray.data());
  
        // Code:
        // movq   rax         , myAddress
        // mov    rcx         , 0x1
        // mov    [rax+rcx*4] , 0xcafe
  
	myFunction.append( {
		0x48, 0xb8                    
	} );

	myFunction.append(myAddress);

	myFunction.append( {
		0x48, 0xc7, 0xc1
	} );

	myFunction.append((ljl::u32)0x1);
  
  
	myFunction.append( {
		0x48, 0xC7, 0x04, 0x88
	} );

	myFunction.append((ljl::u32)0xcafe);

	// Append prolog and make the page executable
	myFunction.make();

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


## Return example (Windows)
```c++
#include <iostream>

#include "LJL\ljl.h"

int main() {
	// Create a function with return type <int>
	ljl::Function <int> myFunction;

        // Code:
        // movq   rax, 0xdeadbeef
        
	myFunction.append( {
		0x48, 0xc7, 0xc0,
	} );

	myFunction.append((ljl::u32)0xdeadbeef);

	// Append prolog and make the page executable
	myFunction.make();

	// Print return value from the generated function
	std::cout << std::hex << myFunction() << std::endl;

	system("pause");
	return 0;
}
```

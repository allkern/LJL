#include "LJL\ljl.h"

#include <iostream>
#include <array>

int main() {
	// Create a function with return type <void>
	ljl::Function <void> myBinary;

	// Create an STL array
	std::array <int, 10> myArray = { 0 };

	// Create an address from a pointer to an array
	ljl::Address myAddress(myArray.data());

	// Append incomplete "movq %rax", pending address
	myBinary.append( { 
		0x48, 0xb8										// movq %rax, ->
	} );

	// Append address
	myBinary.append(myAddress);							// -> address_of(a)

	myBinary.append( {
		0x48, 0xC7, 0xC1								// movq %rcx, ->
	} );

	myBinary.append((ljl::u32)0x8);						// -> 0x3

	myBinary.append( {
		0x48, 0xC7, 0x04, 0x88							// movq [%rax+%rcx*4], ->
	} );

	myBinary.append((ljl::u32)0xcafe);					// -> 0xcafe

	// Append prolog and make the page executable
	myBinary.make();

	// Call the generated function
	myBinary();

	// Print the array
	size_t p = 0;
	for (int a : myArray) {
		std::cout << "myMemory[" << p++ << "] = " << std::hex << a << std::endl;
	}
	system("pause");
	return 0;
}

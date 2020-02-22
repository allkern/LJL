#include "LJL/Function.h"

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <vector>

int main() {
	std::vector <ljl::Bind> myBinds = {
		ljl::Bind((ljl::u64)&std::putchar, "std::putchar"),
		ljl::Bind((ljl::u64)&std::getchar, "std::getchar"),
	};
	ljl::Function <int> myFunction;
	myFunction.append( { 0x55, 0x48, 0x89, 0xE5, 0x49, 0x83, 0x04, 0x9C, 0x41, 0x55, 0x48, 0x89, 0xE5, 0x49, 0x8B, 0x0C, 0x9C, 0x51, 0x48, 0x83, 0xEC, 0x20, 0xFF, 0x15 } );
	myFunction.append(myBinds[0]);
	myFunction.append( { 0x48, 0x83, 0xC4, 0x20, 0x59, 0x5D } );
	myFunction.make();

	for (ljl::u8 b : myFunction) {
		std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)b << " ";
	} std::cout << std::endl;
	system("pause");
	return 0;
}
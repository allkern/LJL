#include "LJL/ljl.h"

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <vector>

int main() {
	ljl::BindTable myBindTable;
	myBindTable.push(ljl::Bind((ljl::u64)&std::putchar, "std::putchar"));
	myBindTable.push(ljl::Bind((ljl::u64)&std::getchar, "std::getchar"));

	ljl::Function <int> myFunction;
	ljl::Builder myBuilder(myFunction, myBindTable);
	myBuilder.append( { 0x48, 0xc7, 0xc0, 0x40, 0x00, 0x00, 0x00 } );
	myBuilder.make();

	for (ljl::u8 b : myFunction) {
		std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)b << " ";
	} std::cout << std::endl;
	return 0;
}
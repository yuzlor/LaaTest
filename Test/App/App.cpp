#include "AsstCaller.h"

#include <iostream>
#include <filesystem>
#include <iostream>
#include <stdio.h>
#include <string>
#include <thread>



int main(int argc, char** argv)
{
	std::cout << "hellop " << std::endl;
	auto ptr = AsstCreate();

	AsstStart(ptr);

	while (AsstRunning(ptr)) {
		std::this_thread::yield();
	}

	AsstStop(ptr);
	return 0;
}
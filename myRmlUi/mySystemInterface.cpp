#include <mySystemInterface.h>

double mySystemInterface::GetElapsedTime() {
	auto stop = std::chrono::high_resolution_clock::now();

	return std::chrono::duration_cast<std::chrono::seconds>(stop - startTime).count();
}

mySystemInterface::mySystemInterface() : SystemInterface{} {

	startTime = std::chrono::high_resolution_clock::now();

	return;
}
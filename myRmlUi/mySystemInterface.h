#pragma once
#include <RmlUi/Core/SystemInterface.h>
#include <chrono>

class mySystemInterface : public Rml::SystemInterface {
	
	std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<__int64, std::ratio<1, 1000000000> > > startTime;

public:
	virtual double GetElapsedTime();

	mySystemInterface();

};
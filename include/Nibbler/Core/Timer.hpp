#pragma once

#include "Nibbler/Core/Core.hpp"

namespace Nibbler
{

class Timer
{
public:
	Timer();
	~Timer() = default;


	void Reset();

	std::chrono::seconds ElapsedSeconds();
	std::chrono::milliseconds ElapsedMS();

private:
	Timer(const Timer&) = delete;
	Timer& operator=(const Timer&) = delete;

private:
	Clock::time_point	_start;
};

} // Nibbler
#pragma once

#include "Nibbler/Core/Core.hpp"

namespace Nibbler
{

class Timer
{
public:
	Timer();
	void	Reset();
	std::chrono::seconds		ElapsedSeconds();
	std::chrono::milliseconds	ElapsedMS();

private:
	Clock::time_point	_start;
};

} // Nibbler
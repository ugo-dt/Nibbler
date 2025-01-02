#include "Nibbler/Core/Timer.hpp"

namespace Nibbler
{

Timer::Timer()
{
	Reset();
}

void	Timer::Reset()
{
	_start = Clock::now();
}

std::chrono::seconds	Timer::ElapsedSeconds()
{
	return std::chrono::duration_cast<std::chrono::seconds>(Clock::now() - _start);
}

std::chrono::milliseconds	Timer::ElapsedMS()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - _start);
}

} // Nibbler

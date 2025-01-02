#include "Nibbler/Core/Log.hpp"

namespace Nibbler
{

void	Log::Init()
{
	if (std::find(_output_streams.begin(), _output_streams.end(), &std::cout) == _output_streams.end())
		_output_streams.emplace_back(&std::cout);
	if (std::find(_error_streams.begin(), _error_streams.end(), &std::cout) == _error_streams.end())
		_error_streams.emplace_back(&std::cerr);
}

} // Nibbler

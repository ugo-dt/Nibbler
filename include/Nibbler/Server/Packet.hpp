#pragma once

#include "Nibbler/Core/Core.hpp"

namespace Nibbler
{

enum class SquareType : uint8_t
{
	Empty = 0,
	Apple      = (1 << 0),
	Snake0     = (1 << 1),
	Snake0Head = (1 << 1) + 1,
	Snake1     = (1 << 2),
	Snake1Head = (1 << 2) + 1,
	Snake2     = (1 << 3),
	Snake2Head = (1 << 3) + 1,
	Snake3     = (1 << 4),
	Snake3Head = (1 << 4) + 1,
};

struct ServerPacket
{
	// todo: id, time
	struct
	{
		SquareType*	squares;
	}payload;
};

} // Nibbler

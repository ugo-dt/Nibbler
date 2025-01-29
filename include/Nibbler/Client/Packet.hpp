#pragma once

#include "Nibbler/Core/Core.hpp"

namespace Nibbler
{

enum class ClientPacketType : uint8_t
{
	None = 0,
	Input, EnableLocalMultiplayer,
	DisableLocalMultiplayer, Disconnect,
};

enum class ClientInput : uint8_t
{
	None = 0,
	Left, Right, Up, Down, Pause,

	// Local multiplayer
	A, D, W, S, // left, right, up, down for the other player
};

struct ClientPacket
{
	uint16_t type;

	struct
	{
		uint16_t input;
	}payload;
};

} // Nibbler

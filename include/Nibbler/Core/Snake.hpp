#pragma once

#include "Nibbler/Core/Core.hpp"

namespace Nibbler
{

enum class Direction
{
	None = 0,
	Left, Right, Up, Down
};

class Snake
{
public:
	Snake();

	void	Init(int8_t game_width, int8_t game_height, uint32_t client_index);

	void	Tick();

	void	SetPos(const i8vec2& pos);
	void	SetDirection(Direction direction);
	void	SetRequestedDirection(Direction direction);
	void	AddSection();

	bool	CollidesWithTail() const;
	bool	HasSectionAt(int8_t x, int8_t y) const;

	const std::vector<i8vec2>& Body() const;

	Direction	GetDirection() const;

	int8_t	PosX() const;
	int8_t	PosY() const;

	size_t	size() const;

	std::vector<i8vec2>::const_iterator	begin() const;
	std::vector<i8vec2>::const_iterator	end() const;

private:
	std::vector<i8vec2>	_body;
	Direction			_direction;
	Direction			_requested_direction;
};

} // Nibbler

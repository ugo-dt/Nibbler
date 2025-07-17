#include "Nibbler/Core/Snake.hpp"

namespace Nibbler
{

Snake::Snake()
	: _body(),
	  _direction(Direction::Right),
	  _requested_direction(Direction::Right),
	  _dead(false)
{
}

void	Snake::Init(int8_t game_width, int8_t game_height, uint32_t client_index)
{
	_dead = false;
	i8vec2 head_pos = i8vec2(
		(int8_t)(game_width / 2),
		(int8_t)(game_height / 2 + client_index)
	);

	if (game_width % 2 == 0)
		head_pos.x -= 1;
	if (game_height % 2 == 0)
		head_pos.y -= 1;

	_body.clear();
	_body.resize(4);
	_body[0] = head_pos;

	for (int8_t i = 1; i < SNAKE_DEFAULT_SIZE; i++)
	{
		_body[i].x = (int8_t)(_body[i - 1].x - 1);
		_body[i].y = _body[i - 1].y;
	}
	_direction = Direction::Right;
	_requested_direction = Direction::Right;
	_timer.Reset();
	_can_collide = false;
}

void	Snake::Tick(
	int8_t game_width,
	int8_t game_height,
	uint32_t client_index,
	std::chrono::milliseconds death_timer_ms,
	std::chrono::milliseconds no_collisions_timer_ms
) {
	if (_dead)
	{
		if (_timer.ElapsedMS() > std::chrono::milliseconds(death_timer_ms))
			Init(game_width, game_height, client_index);
		return ;
	}
	if (!_can_collide && _timer.ElapsedMS() > std::chrono::milliseconds(no_collisions_timer_ms))
		_can_collide = true;

	_direction = _requested_direction;

    for (size_t i = _body.size() - 1; i > 0; i--)
    {
        _body[i].x = _body[i - 1].x;
        _body[i].y = _body[i - 1].y;
    }

    i8vec2& head = _body[0];
    switch (_direction)
    {
        case Direction::Left:
            head.x -= 1;
            break;
        case Direction::Right:
            head.x += 1;
            break;
        case Direction::Up:
            head.y -= 1;
            break;
        case Direction::Down:
            head.y += 1;
            break;
		default:
			break;
    }
}

void	Snake::Kill()
{
	if (_dead)
		return ;
	_dead = true;
	_timer.Reset();
	_body.clear();
}

void	Snake::SetPos(const i8vec2& pos)
{
	NIB_ASSERT(!_body.empty());
	_body[0] = pos;
}

void	Snake::SetDirection(Direction direction)
{
	_direction = direction;
}

void	Snake::SetRequestedDirection(Direction direction)
{
	_requested_direction = direction;
}

void	Snake::AddSection()
{
	_body.emplace_back(_body[_body.size() - 1]);
}

bool	Snake::CollidesWithTail() const
{
	for (size_t i = 1; i < _body.size(); i++)
		if (_body[i].x == _body[0].x && _body[i].y == _body[0].y)
			return true;
	return false;
}

bool	Snake::HasSectionAt(int8_t x, int8_t y) const
{
	for (size_t i = 0; i < _body.size(); i++)
		if (_body[i].x == x && _body[i].y == y)
			return true;
	return false;
}

bool	Snake::IsPositionInTail(int8_t x, int8_t y) const
{
	for (size_t i = 1; i < _body.size(); i++)
		if (_body[i].x == x && _body[i].y == y)
			return true;
	return false;
}

bool	Snake::CanCollide() const
{
	return _can_collide;
}

bool	Snake::Dead() const
{
	return _dead;
}

const std::vector<i8vec2>& Snake::Body() const
{
	return _body;
}

Direction	Snake::GetDirection() const
{
	return _direction;
}

int8_t	Snake::PosX() const
{
	return _body[0].x;
}

int8_t	Snake::PosY() const
{
	return _body[0].y;
}

size_t	Snake::size() const
{
	return _body.size();
}

std::vector<i8vec2>::const_iterator	Snake::begin() const
{
	return _body.begin();
}

std::vector<i8vec2>::const_iterator	Snake::end() const
{
	return _body.end();
}

} // Nibbler

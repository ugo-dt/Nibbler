#include "Nibbler/Core/Game.hpp"
#include "Nibbler/Server/Server.hpp"

namespace Nibbler
{

Game::Game(
	int8_t width,
	int8_t height,
	bool pvp,
	std::chrono::milliseconds death_timer_ms,
	std::chrono::milliseconds no_collisions_timer_ms
) : _state(),
	_pause(true),
	_rd(),
	_rng(_rd()),
	_distX(0, width - 1),
	_distY(0, height - 1)
{
	_state.width = width;
	_state.height = height;
	_state.pvp = pvp;
	_state.death_timer_ms = death_timer_ms;
	_state.no_collisions_timer_ms = no_collisions_timer_ms;
	for (uint32_t i = 0; i < MAX_CLIENTS; i++)
		_state.snakes[i].Init(_state.width, _state.height, i);
	SpawnApple();
}

void	Game::ResetSnake(uint32_t client_index)
{
	_state.snakes[client_index].Init(_state.width, _state.height, client_index);
}

void	Game::AddLocalPlayer(uint32_t index)
{
	ResetSnake(index + MAX_CLIENTS);
}

void	Game::TogglePvP()
{
	_state.pvp = !_state.pvp;
}

void	Game::CheckSnakeCollisions(uint32_t snake_index)
{
	Snake& snake = _state.snakes[snake_index];

	if (snake.Dead() || snake.size() == 0)
		return ;

	if (snake.PosX() < 0 || snake.PosY() < 0 || snake.PosX() >= _state.width || snake.PosY() >= _state.height
		|| snake.CollidesWithTail())
	{
		KillSnake(snake_index);
	}

	if (_state.pvp)
	{
		for (uint32_t i = 0; i < _state.snakes.size(); i++)
		{
			if (i == snake_index)
				continue;

			// Snakes are allowed to overlap with other snake heads.
			// They cannot overlap with a tail.
			auto& other_snake = _state.snakes[i];

			if (other_snake.Dead())
				continue;

			if (snake.CanCollide() && other_snake.CanCollide()
				&& other_snake.IsPositionInTail(snake.PosX(), snake.PosY()))
			{
				// Kill both snakes
				if (snake.IsPositionInTail(other_snake.PosX(), other_snake.PosY()))
					KillSnake(i);
				KillSnake(snake_index);
			}
		}
	}

	if (snake.size() >= (size_t)(_state.width * _state.height))
	{
		Log::Info("Snake {} won!", snake_index);
		KillSnake(snake_index);
	}

	if (!snake.Dead() && snake.PosX() == _state.apple.x && snake.PosY() == _state.apple.y)
	{
		snake.AddSection();
		SpawnApple();
	}
}

void	Game::Tick()
{
	if (_pause)
		return ;

	for (uint32_t i = 0; i < MAX_CLIENTS; i++)
	{
		if (!Server::ClientExists(i))
			continue;

		_state.snakes[i].Tick(_state.width, _state.height, i,
			_state.death_timer_ms, _state.no_collisions_timer_ms);
		if (Server::ClientHasLocalMultiplayer(i))
			_state.snakes[i + MAX_CLIENTS].Tick(_state.width, _state.height,i + MAX_CLIENTS,
				_state.death_timer_ms, _state.no_collisions_timer_ms);

		CheckSnakeCollisions(i);
		if (Server::ClientHasLocalMultiplayer(i))
			CheckSnakeCollisions(i + MAX_CLIENTS);
	}
}

void	Game::SetSnakeRequestedDirection(uint32_t index, Direction direction)
{
	switch (direction)
	{
		case Direction::Left:
			if (_state.snakes[index].GetDirection() != Direction::Right)
				_state.snakes[index].SetRequestedDirection(direction);
			break;
		case Direction::Right:
			if (_state.snakes[index].GetDirection() != Direction::Left)
				_state.snakes[index].SetRequestedDirection(direction);
			break;
		case Direction::Up:
			if (_state.snakes[index].GetDirection() != Direction::Down)
				_state.snakes[index].SetRequestedDirection(direction);
			break;
		case Direction::Down:
			if (_state.snakes[index].GetDirection() != Direction::Up)
				_state.snakes[index].SetRequestedDirection(direction);
			break;
		default:
			break;
	}
}

void	Game::KillSnake(uint32_t index)
{
	_state.snakes[index].Kill();
}

void	Game::Pause()
{
	_pause = true;
}

void	Game::Resume()
{
	_pause = false;
}

void	Game::TogglePause()
{
	_pause = !_pause;
}

bool	Game::IsPvPEnabled() const
{
	return _state.pvp;
}

bool	Game::Paused() const
{
	return _pause;
}

void	Game::SpawnApple()
{
	bool validPosition = false;

	for (uint32_t i = 0; i < MAX_CLIENTS; i++)
		if (_state.snakes[i].size() >= (size_t)(_state.width * _state.height))
			return ;

	int i = 0;
	while (!validPosition && i < 500)
	{
		_state.apple.x = _distX(_rng);
		_state.apple.y = _distY(_rng);
		validPosition = true;

		for (uint32_t i = 0; i < MAX_CLIENTS; i++)
		{
			// Ensure the apple doesn't overlap with the snake
			for (const auto& section : _state.snakes[i])
			{
				if (section.x == _state.apple.x && section.y == _state.apple.y)
				{
					validPosition = false;
					break;
				}
			}

			if (Server::ClientHasLocalMultiplayer(i))
			{
				for (const auto& section : _state.snakes[i + MAX_CLIENTS])
				{
					if (section.x == _state.apple.x && section.y == _state.apple.y)
					{
						validPosition = false;
						break;
					}
				}
			}
		}
		i++;
	}
}

const GameState& Game::GetState() const
{
	return _state;
}

} // Nibbler

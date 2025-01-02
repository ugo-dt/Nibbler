#include "Nibbler/Core/Game.hpp"
#include "Nibbler/Server/Server.hpp"

namespace Nibbler
{

Game::Game(int8_t width, int8_t height)
	: _state(),
	  _pause(true),
	  _rd(),
	  _rng(_rd()),
	  _distX(0, width - 1),
	  _distY(0, height - 1)
{
	_state.width = width;
	_state.height = height;
	for (uint32_t i = 0; i < MAX_CLIENTS; i++)
		_state.snakes[i].Init(_state.width, _state.height, i);
	SpawnApple();
}

void	Game::AddLocalPlayer(uint32_t index)
{
	_state.snakes[index + MAX_CLIENTS].Init(_state.width, _state.height, index + MAX_CLIENTS);
}

void	Game::CheckSnakeCollisions(uint32_t snake_index)
{
	Snake& snake = _state.snakes[snake_index];

	if (snake.PosX() < 0 || snake.PosY() < 0 || snake.PosX() >= _state.width || snake.PosY() >= _state.height
		|| snake.CollidesWithTail())
	{
		KillSnake(snake_index);
	}

	if (snake.size() >= (size_t)(_state.width * _state.height))
	{
		Log::Info("Snake {} won!");
		KillSnake(snake_index);
	}
	
	if (snake.PosX() == _state.apple.x && snake.PosY() == _state.apple.y)
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

		_state.snakes[i].Tick();
		CheckSnakeCollisions(i);

		if (Server::ClientHasLocalMultiplayer(i))
		{
			_state.snakes[i + MAX_CLIENTS].Tick();
			CheckSnakeCollisions(i + MAX_CLIENTS);
		}
	}
}

void	Game::ResetSnake(uint32_t client_index)
{
	_state.snakes[client_index].Init(_state.width, _state.height, client_index);
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
	_state.snakes[index].Init(_state.width, _state.height, index);
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

bool	Game::Paused()
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

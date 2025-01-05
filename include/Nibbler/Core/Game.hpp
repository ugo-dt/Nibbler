#pragma once

#include "Nibbler/Core/Core.hpp"
#include "Nibbler/Core/Snake.hpp"

namespace Nibbler
{

struct GameState
{
	int8_t	width;
	int8_t	height;
	std::array<Snake, MAX_CLIENTS * 2> snakes; // We multiply by 2 to allow local mulltiplayer for each client.
	i8vec2	apple;
};

class Game
{
public:
	Game(int8_t width, int8_t height);
	~Game() = default;

	Game(Game& game) = delete;
	Game& operator=(Game& game) = delete;

	void	AddLocalPlayer(uint32_t index);

	void	Pause();
	void	Resume();
	void	TogglePause();
	bool	Paused();

	void	Tick();

	void	ResetSnake(uint32_t client_index);
	void	SetSnakeRequestedDirection(uint32_t index, Direction direction);

	const GameState& GetState() const;

private:
	void	CheckSnakeCollisions(uint32_t snake_index);
	void	KillSnake(uint32_t index);
	void	SpawnApple();

private:
	GameState	_state;
	bool		_pause;

	std::random_device	_rd;
	std::mt19937		_rng;
    std::uniform_int_distribution<int8_t> _distX, _distY;
};

} // Nibbler

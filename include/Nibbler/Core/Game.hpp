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
	bool	pvp; // if true, snakes can kill each other
	std::chrono::milliseconds	death_timer_ms;
	std::chrono::milliseconds	no_collisions_timer_ms;
};

class Game
{
public:
	Game(
		int8_t width,
		int8_t height,
		bool pvp,
		std::chrono::milliseconds death_timer_ms,
		std::chrono::milliseconds no_collisions_timer_ms
	);
	~Game() = default;

	Game(Game& game) = delete;
	Game& operator=(Game& game) = delete;

	void	AddLocalPlayer(uint32_t index);

	void	Pause();
	void	Resume();
	void	Tick();
	void	TogglePause();
	void	TogglePvP();

	bool	Paused() const;
	bool	IsPvPEnabled() const;

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

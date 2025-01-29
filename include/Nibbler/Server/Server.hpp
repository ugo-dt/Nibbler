#pragma once

#include "Nibbler/Core/Core.hpp"
#include "Nibbler/Core/Game.hpp"
#include "Nibbler/Client/Packet.hpp"
#include "Nibbler/Server/Packet.hpp"

namespace Nibbler
{

struct ServerClient
{
	int8_t					id;
	FixedQueue<ClientInput>	input_queue;

	bool					with_local_mutliplayer;
	FixedQueue<ClientInput>	input_queue_local_mutliplayer;
};

struct ServerConfig
{
	const char*	host;
	int			port;
	int8_t		game_width;
	int8_t		game_height;
	uint64_t	ns_per_tick;
};

class Server
{
public:
	static void	Init(const ServerConfig& config);
	static void	Shutdown(int signum = 0);

	static void	Run();
	static void	Stop();

	static bool	Running();

	static void	CloseSocket(NibblerSocket socket);

	static bool	ClientExists(uint32_t index);
	static void	AddLocalPlayer(uint32_t client_index);
	static void	RemoveLocalPlayer(uint32_t client_index);
	static bool	ClientHasLocalMultiplayer(uint32_t client_index);

	static void	RenderImGuiDebug();

private:
	static void	Tick();
	static void	Update();
	static void	AddInputToQueue(uint32_t client_index, ClientInput input);
	static void	ProcessClientsInputs();

	static void	BindSocketAndListen(const char *host, const int port);

	static void	AcceptClient();
	static void	RemoveClient(NibblerSocket socket);

	static void	Broadcast();
	static void	ReceivePacket(NibblerSocket socket);

	static std::array<ServerClient, MAX_CLIENTS>::iterator GetClientFromSocket(NibblerSocket socket);

private:
	static bool				_running;
	static std::mutex		_running_mutex;
	static int64_t			_ns_per_tick;
	static Scope<Game>		_game;

	static std::string		_host;
	static int				_port;
	static NibblerSocket	_socket;
	static pollfd*			_poll_fds;
	static NibblerNfds_t	_nfds;

	static uint32_t			_client_count;
	static std::array<ServerClient, MAX_CLIENTS>	_clients;

	static int	_fps, _tps;
};

} // Nibbler

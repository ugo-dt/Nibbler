#include "Nibbler/Server/Server.hpp"
#include "Nibbler/Client/Client.hpp"

namespace Nibbler
{

bool									Server::_running = false;
std::mutex								Server::_running_mutex;
int64_t									Server::_ns_per_tick = 0;
std::string								Server::_host;
int										Server::_port = -1;
NibblerSocket							Server::_socket = INVALID_SOCKET;
NibblerNfds_t							Server::_nfds = 0;
pollfd*									Server::_poll_fds = nullptr;
Scope<Game>								Server::_game = nullptr;
uint32_t								Server::_client_count;
std::array<ServerClient, MAX_CLIENTS>	Server::_clients;
int										Server::_fps;
int										Server::_tps;

void	Server::Init(const ServerConfig& config)
{
	int status;
	int opt;

	NIB_NOTUSED(status);
	std::signal(SIGINT, Shutdown);
	std::signal(SIGTERM, Shutdown);

	_ns_per_tick = config.ns_per_tick;

	_socket = socket(AF_INET, SOCK_STREAM, 0);
	NIB_ASSERT(_socket != INVALID_SOCKET, "socket(): {}", GetLastNetworkError());

	opt = 1;
	status = setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(int));
	NIB_ASSERT(status != -1, "setsockopt(): {}", GetLastNetworkError());

	Log::Info("[SERVER] Created socket {}", _socket);

	_nfds = MAX_CLIENTS + 1;
	_poll_fds = new pollfd[_nfds];
	_poll_fds[0].fd = _socket;
	_poll_fds[0].events = POLLRDNORM;

	for (NibblerNfds_t i = 1; i < _nfds; i++)
	{
		_poll_fds[i].fd = INVALID_SOCKET;
		_poll_fds[i].events = POLLRDNORM | POLLWRNORM; // Equivalent to POLLIN | POLLOUT
	}

	_game = CreateScope<Game>(config.game_width, config.game_height);

	BindSocketAndListen(config.host, config.port);
	
	Log::Info("[SERVER] Initialized.");
}

void	Server::BindSocketAndListen(const char *host, const int port)
{
	int status;
	struct sockaddr_in addr;

	NIB_NOTUSED(status);
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	if (std::strcmp(host, "localhost") == 0)
		host = "127.0.0.1";
	addr.sin_addr.s_addr = inet_addr(host);
	addr.sin_port = htons(port);
	status = bind(_socket, (struct sockaddr *)&addr, sizeof(addr));
	NIB_ASSERT(status != -1, "bind(): {}", GetLastNetworkError());
	Log::Info("[SERVER] Bound socket {}", _socket);

#ifdef _WIN32
	u_long mode = 1;
	status = ioctlsocket(_socket, FIONBIO, &mode);
	NIB_ASSERT(status == NO_ERROR, "ioctlsocket(): {}", GetLastNetworkError());
#else
	status = fcntl(_socket, F_SETFL, O_NONBLOCK);
	NIB_ASSERT(status != -1, "fcntl(): {}", GetLastNetworkError());
#endif

	status = listen(_socket, MAX_CLIENTS);
	NIB_ASSERT(status != -1, "listen(): {}", GetLastNetworkError());

	_host = inet_ntoa(addr.sin_addr);
	_port = port;
}

void	Server::CloseSocket(NibblerSocket socket)
{
#ifdef _WIN32
	closesocket(socket);
#else
	close(socket);
#endif
}

bool	Server::Running()
{
	bool running;

	_running_mutex.lock();
	running = _running;
	_running_mutex.unlock();
	return running;
}

void	Server::Shutdown(int signum)
{
	NIB_NOTUSED(signum);

	if (Running())
		Stop();
	if (!_poll_fds)
		return ;
	std::signal(SIGINT, SIG_DFL);
	std::signal(SIGTERM, SIG_DFL);

	for (NibblerNfds_t i = 1; i < _nfds; i++)
		if (_poll_fds[i].fd != INVALID_SOCKET)
			CloseSocket(_poll_fds[i].fd);
	delete[] _poll_fds;
	_poll_fds = nullptr;

	if (_socket != INVALID_SOCKET)
	{
		CloseSocket(_socket);
		_socket = -1;
	}

	Log::Info("[SERVER] Shutdown.");
	g_ApplicationRunning = false;
}

void	Server::AddInputToQueue(uint32_t client_index, ClientInput input)
{
	switch (input)
	{
		case ClientInput::Pause:
		case ClientInput::Left:
		case ClientInput::Right:
		case ClientInput::Up:
		case ClientInput::Down:
			_clients[client_index].input_queue.push(input);
			break;
		case ClientInput::A:
		case ClientInput::D:
		case ClientInput::W:
		case ClientInput::S:
			if (_clients[client_index].with_local_mutliplayer)
				_clients[client_index].input_queue_local_mutliplayer.push(input);
			break;
		default:
			break;
	}
}

void	Server::ProcessClientsInputs()
{
	for (auto& client : _clients)
	{
		while (!client.input_queue.empty())
		{
			if (client.input_queue.front() == ClientInput::Pause && client.id == 0)// && _clients.size() == 1)
				_game->TogglePause();
			else if (!_game->Paused())
			{
				switch (client.input_queue.front())
				{
					case ClientInput::Left:  _game->SetSnakeRequestedDirection(client.id, Direction::Left); break;
					case ClientInput::Right: _game->SetSnakeRequestedDirection(client.id, Direction::Right); break;
					case ClientInput::Up:    _game->SetSnakeRequestedDirection(client.id, Direction::Up); break;
					case ClientInput::Down:  _game->SetSnakeRequestedDirection(client.id, Direction::Down); break;
					default: break;
				}
			}
			client.input_queue.pop();
		}

		while (!client.input_queue_local_mutliplayer.empty())
		{
			if (!_game->Paused())
			{
				switch (client.input_queue_local_mutliplayer.front())
				{
					case ClientInput::A: _game->SetSnakeRequestedDirection(client.id + MAX_CLIENTS, Direction::Left); break;
					case ClientInput::D: _game->SetSnakeRequestedDirection(client.id + MAX_CLIENTS, Direction::Right); break;
					case ClientInput::W: _game->SetSnakeRequestedDirection(client.id + MAX_CLIENTS, Direction::Up); break;
					case ClientInput::S: _game->SetSnakeRequestedDirection(client.id + MAX_CLIENTS, Direction::Down); break;
					default: break;
				}
			}
			client.input_queue_local_mutliplayer.pop();
		}
	}
}

void	Server::Tick()
{
	_game->Tick();
	Broadcast();
}

void	Server::Update()
{
	int poll_ret;

#ifdef _WIN32
	poll_ret = WSAPoll(_poll_fds, _nfds, 0);
#else
	poll_ret = poll(_poll_fds, _nfds, 0);
#endif
	NIB_ASSERT(poll_ret != -1, "poll: {}", GetLastNetworkError());

	if (poll_ret > 0)
	{
		if (_poll_fds[0].revents & POLLIN)
			AcceptClient();
		for (NibblerNfds_t i = 1; i < _nfds; i++)
		{
			if (_poll_fds[i].fd == INVALID_SOCKET)
				continue;
			if (_poll_fds[i].revents & (POLLNVAL | POLLHUP | POLLERR))
			{
				RemoveClient(_poll_fds[i].fd);
				_poll_fds[i].revents = 0;
				continue;
			}
			if (_poll_fds[i].revents & POLLRDNORM)
			{
				ReceivePacket(_poll_fds[i].fd);
			}
		}
	}

	ProcessClientsInputs();
}

void	Server::Stop()
{
	_running_mutex.lock();
	_running = false;
	_running_mutex.unlock();
	
	Log::Info("[SERVER] Stop.");
}

void	Server::Run()
{
	bool	running;

	auto lastFrameTime = Clock::now();
	auto lastSecondTime = lastFrameTime;
	long long tickRemainder = 0;

	int frames = 0, ticks = 0;

	_running_mutex.lock();
	_running = true;
	running = true;
	_running_mutex.unlock();
	
	Log::Info("[SERVER] Listening on {}:{}", _host, _port);
	while (running)
	{
		auto currentTime = Clock::now();
		auto elapsedTime = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - lastFrameTime).count();
		lastFrameTime = currentTime;

		if (std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - lastSecondTime).count() >= NS_PER_SECOND)
		{
			_fps = frames;
			_tps = ticks;
			frames = ticks = 0;
			lastSecondTime = currentTime;
		}

		tickRemainder += elapsedTime;
		while (tickRemainder >= _ns_per_tick)
		{
			Tick();
			tickRemainder -= _ns_per_tick;
			ticks++;
		}

		Update();
		frames++;

		// Cap framerate
		auto frameEndTime = Clock::now();
		auto frameDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(frameEndTime - lastFrameTime).count();
		if (frameDuration < NS_PER_FRAME)
		{
			auto sleepDuration = std::chrono::nanoseconds(NS_PER_FRAME - frameDuration);
			std::this_thread::sleep_for(sleepDuration);
		}

		_running_mutex.lock();
		running = _running;
		_running_mutex.unlock();
	}
}

void	Server::RenderImGuiDebug()
{
	ImGui::SetNextWindowBgAlpha(0.7f); // Transparent background
	ImGui::Begin("Nibbler Server");
	ImGui::SeparatorText("Server");
	ImGui::Text("Address: %s:%d", _host.c_str(), _port);
	ImGui::Text("FPS: %d | TPS: %d", _fps, _tps);
	ImGui::Text("Clients: %u", _client_count);
	if (ImGui::Button(_game->Paused() ? "Resume" : "Pause"))
		_game->TogglePause();
	ImGui::SameLine();
	if (ImGui::Button("Tick"))
	{
		_game->Resume();
		Tick();
		_game->Pause();
	}
	// ImGui::SameLine();
	// if (ImGui::Button("Update"))
	// 	Update();
	ImGui::End();
}

} // Nibbler

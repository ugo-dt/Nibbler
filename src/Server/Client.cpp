#include "Nibbler/Server/Server.hpp"
#include "Nibbler/Client/Client.hpp"

namespace Nibbler
{

void	Server::AcceptClient()
{
	struct sockaddr_in addr = {};
	size_t addr_len = sizeof(addr);

	NibblerSocket socket = accept(_socket, (struct sockaddr *)&addr, (socklen_t *)&addr_len);
	if (socket == INVALID_SOCKET)
	{
		Log::Warn("[SERVER] Refused new client: {}", GetLastNetworkError());
		return ;
	}
	NibblerNfds_t i = 1;
	while (i < _nfds)
	{
		if (_poll_fds[i].fd == INVALID_SOCKET)
		{
			_client_count++;
			_poll_fds[i].fd = socket;
			_poll_fds[i].revents = 0;

			// Clear the input queue
			_clients[i - 1].id = i - 1;
			FixedQueue<ClientInput>().swap(_clients[i - 1].input_queue);

			Log::Info("[SERVER] Accepted new client: {} (socket {})", i - 1, socket);

			struct { uint16_t id, width, height; } handshake;
			handshake.id = htons(i - 1);
			handshake.width = htons(_game->GetState().width);
			handshake.height = htons(_game->GetState().height);

			send(socket, (const char *)&handshake, sizeof(handshake), 0);
			return ;
		}
		i++;
	}
	send(socket, "too many clients", 17, 0);
	Log::Warn("[SERVER] Refused new client: too many clients");
	CloseSocket(socket);
}

std::array<ServerClient, MAX_CLIENTS>::iterator
Server::GetClientFromSocket(NibblerSocket socket)
{
	for (NibblerNfds_t i = 1; i < _nfds; i++)
		if (_poll_fds[i].fd == socket)
			return _clients.begin() + (i - 1);

	Log::Warn("[SERVER] GetClientFromSocket(): could not find client with socket {}", socket);
	return _clients.end();
}

void	Server::RemoveClient(NibblerSocket socket)
{
	for (NibblerNfds_t i = 1; i < _nfds; i++)
	{
		if (_poll_fds[i].fd == socket)
		{
			CloseSocket(socket);
			_poll_fds[i].fd = INVALID_SOCKET;
			_client_count--;

			_game->ResetSnake(i - 1);
			RemoveLocalPlayer(i - 1);

			Log::Info("[SERVER] Removed client {} (socket {})", i - 1, socket);
			return ;
		}
	}
	Log::Warn("[SERVER] RemoveClient(): could not find client with socket {}", socket);
}

void	Server::Broadcast()
{
	const GameState& state = _game->GetState();

	SquareType squares[state.width][state.height];

	std::memset(&squares, 0, sizeof(SquareType) * state.width * state.height);
	squares[state.apple.x][state.apple.y] = SquareType::Apple;

	for (uint32_t i = 0; i < MAX_CLIENTS; i++)
	{
		if (!ClientExists(i))
			continue;
		
		if (!state.snakes[i].Dead())
		{
			auto& snake = state.snakes[i].Body();
			squares[snake[0].x][snake[0].y] = (SquareType)((1 << (i + 1)) + 1);
			for (size_t j = 1; j < snake.size(); j++)
			squares[snake[j].x][snake[j].y] = (SquareType)((1 << (i + 1)));
		}

		if (_clients[i].with_local_mutliplayer)
		{
			if (!state.snakes[i + MAX_CLIENTS].Dead())
			{
				const auto& snake = state.snakes[i + MAX_CLIENTS].Body();
				squares[snake[0].x][snake[0].y] = (SquareType)((1 << (i + MAX_CLIENTS + 1)) + 1);
				for (size_t j = 1; j < snake.size(); j++)
				squares[snake[j].x][snake[j].y] = (SquareType)((1 << (i + MAX_CLIENTS + 1)));
			}
		}
	}

	for (NibblerNfds_t i = 1; i < _nfds; i++)
	{
		if (_poll_fds[i].fd == INVALID_SOCKET)
			continue;
		if (_poll_fds[i].revents & POLLWRNORM)
			if (send(_poll_fds[i].fd, (const char *)squares, sizeof(SquareType) * state.width * state.height, 0) <= 0)
				RemoveClient(_poll_fds[i].fd);
	}
}

void	Server::ReceivePacket(NibblerSocket socket)
{
	ClientPacket	packet = {};

	auto it_client = GetClientFromSocket(socket);
	if (it_client == _clients.end())
		return ;

	ssize_t bytes = recv(socket, (char *)&packet, sizeof(ClientPacket), 0);
	if (bytes <= 0)
	{
		Log::Warn("[SERVER] Lost connection with client {}", socket);
		RemoveClient(socket);
		return ;
	}

	ClientPacketType type = ClientPacketType(ntohs(packet.type));

	switch (type)
	{
		case ClientPacketType::Input:
			AddInputToQueue(it_client->id, ClientInput(ntohs(packet.payload.input)));
			break;
		case ClientPacketType::EnableLocalMultiplayer:
			AddLocalPlayer(it_client->id);
			break;
		case ClientPacketType::DisableLocalMultiplayer:
			RemoveLocalPlayer(it_client->id);
			break;
		default:
			break;
	}
}

bool	Server::ClientExists(uint32_t client_index)
{
	return _poll_fds[client_index + 1].fd != INVALID_SOCKET;
}

bool	Server::ClientHasLocalMultiplayer(uint32_t client_index)
{
	return _clients[client_index].with_local_mutliplayer;
}

void	Server::AddLocalPlayer(uint32_t client_index)
{
	if (!ClientExists(client_index))
	{
		Log::Warn("[SERVER] AddLocalPlayer(): no client with id {}", client_index);
		return ;
	}

	ServerClient& client = _clients[client_index];
	
	if (client.with_local_mutliplayer)
		return ;
	client.with_local_mutliplayer = true;

	_game->AddLocalPlayer(client_index);

	Log::Info("[SERVER] Enabled local multiplayer for client {}", client_index);
}

void	Server::RemoveLocalPlayer(uint32_t client_index)
{
	if (!ClientExists(client_index))
	{
		Log::Warn("[SERVER] AddLocalPlayer(): no client with id {}", client_index);
		return ;
	}
	
	if (_clients[client_index].with_local_mutliplayer)
	{
		_game->ResetSnake(client_index + MAX_CLIENTS);
		_clients[client_index].with_local_mutliplayer = false;
	}
	
	Log::Info("[SERVER] Disabled local multiplayer for client {}", client_index);
}

} // Nibbler

#include "Nibbler/Client/Client.hpp"
#include "Nibbler/Server/Server.hpp"

namespace Nibbler
{

void	Client::Connect(const char *host, const int port)
{
	struct sockaddr_in addr;
	int status;
	int opt;

	NIB_NOTUSED(status);

	_socket = socket(AF_INET, SOCK_STREAM, 0);
	NIB_ASSERT(_socket != INVALID_SOCKET, "socket(): {}", GetLastNetworkError());
	
	if (_socket == INVALID_SOCKET)
		return ;
	
	Log::Info("[CLIENT] Created socket {}", _socket);

	opt = 1;
	status = setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(int));
	NIB_ASSERT(status != -1, "setsockopt(): {}", GetLastNetworkError());

	addr.sin_family = AF_INET;
	if (std::strcmp(host, "localhost") == 0)
		host = "127.0.0.1";
	addr.sin_addr.s_addr = inet_addr(host);
	addr.sin_port = htons(port);
	
	status = connect(_socket, (sockaddr *)&addr, sizeof(addr));
	NIB_ASSERT(status != -1, "connect(): {}: {}", status, GetLastNetworkError());

	if (status == -1)
		return ;

	struct { uint16_t id, width, height; } handshake;
	recv(_socket, (char *)&handshake, sizeof(handshake), 0);

	_id = ntohs(handshake.id);
	_game.width = ntohs(handshake.width);
	_game.height = ntohs(handshake.height);

	Log::Info("[CLIENT] Connected with server at {}:{} (socket: {})", host, port, _socket);
	Log::Info("[CLIENT] ID: {}", _id);

	_nfds = 1;
	_poll_fds = new pollfd[_nfds];
	_poll_fds[0].fd = _socket;
	_poll_fds[0].events = POLLRDNORM | POLLWRNORM;

	return;
}

void	Client::Disconnect()
{
	if (_poll_fds)
	{
		for (size_t i = 0; i < _nfds; i++)
			if (_poll_fds[i].fd != INVALID_SOCKET)
				Server::CloseSocket(_poll_fds[i].fd);
		Log::Info("[CLIENT] Disconnected.");
		delete[] _poll_fds;
		_poll_fds = nullptr;
	}
	if (_socket != INVALID_SOCKET)
	{
		Server::CloseSocket(_socket);
		Log::Info("[CLIENT] Closed socket {}", _socket);
		_socket = INVALID_SOCKET;
	}
}

void	Client::SendPacket(ClientPacket& packet)
{
	if (_poll_fds[0].revents & (POLLOUT))
		if (send(_socket, (const char *)&packet, sizeof(struct ClientPacket), 0) < 0)
			Log::Warn("[CLIENT] SendPacket(): {}", GetLastNetworkError());
}

void	Client::SendInputPacket(ClientInput input)
{
	ClientPacket	packet = {};

	packet.type = htons((uint16_t)ClientPacketType::Input);
	packet.payload.input = htons((uint16_t)input);
	SendPacket(packet);
}

void	Client::SendEnableMultiplayerPacket()
{
	ClientPacket	packet = {};

	packet.type = htons((uint16_t)ClientPacketType::EnableLocalMultiplayer);
	SendPacket(packet);
}

void	Client::ReceivePacket()
{
#ifdef _WIN32
	int poll_ret = WSAPoll(_poll_fds, _nfds, 0);
#else
	int poll_ret = poll(_poll_fds, _nfds, 0);
#endif
	NIB_ASSERT(poll_ret != -1, "poll(): {}", GetLastNetworkError());

	if (poll_ret > 0)
	{
		if (_poll_fds[0].fd == INVALID_SOCKET)
			return ;
		if (_poll_fds[0].revents & (POLLNVAL | POLLHUP | POLLERR))
		{
			Log::Warn("[CLIENT] Connection lost.\n");
			Disconnect();
			return ;
		}
		if (_poll_fds[0].revents & (POLLRDNORM))
		{
			SquareType squares[_game.width][_game.height];
			ssize_t bytes = 0;
			bytes = recv(_socket, (char *)squares, sizeof(SquareType) * _game.width * _game.height, 0);
			if (bytes < 0)
			{
				Log::Warn("[CLIENT] ReceivePacket(): {}", GetLastNetworkError());
				return ;
			}
			if (bytes == 0)
			{
				Log::Warn("[CLIENT] Connection closed.\n");
				Disconnect();
			}
			else
			{
				for (int x = 0; x < _game.width; x++)
					for (int y = 0; y < _game.width; y++)
						_game.squares[x][y] = squares[x][y];
			}
		}
	}
}

} // Nibbler

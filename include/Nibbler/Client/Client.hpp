#pragma once

#include "Nibbler/Core/Core.hpp"
#include "Nibbler/Core/Timer.hpp"
#include "Nibbler/Core/Snake.hpp"
#include "Nibbler/Client/Packet.hpp"
#include "Nibbler/Events/ApplicationEvent.hpp"
#include "Nibbler/Events/KeyEvent.hpp"
#include "Nibbler/Renderer/Renderer.hpp"
#include "Nibbler/Server/Packet.hpp"

namespace Nibbler
{

std::string	GetLastNetworkError();

struct ClientGame
{
	int8_t		width;
	int8_t		height;
	std::vector<std::vector<SquareType>>	squares;
};

struct ClientConfig
{
	RendererAPI::API api; 
	const char* host;
	int port;
	std::chrono::seconds timeout;
};

class Client
{
public:
	Client(const ClientConfig& config);
	~Client();

	void	Run();

private:
	void	InitRenderer(RendererAPI::API api);

	void	Update();
	void	Render();
	void	RenderImGui();

	void	Close();

	void	Connect(const char *host, const int port, std::chrono::seconds timeout);
	void	Disconnect();

	void	SendPacket(ClientPacket& packet);
	void	SendEnableMultiplayerPacket();
	void	SendDisableMultiplayerPacket();
	void	SendInputPacket(ClientInput input);

	void	ReceivePacket();
	
	bool	OnEvent(Nibbler::Event& event);
	bool	OnKeyPressed(Nibbler::KeyPressedEvent& event);
	bool	OnWindowClose(WindowCloseEvent& event);
	bool	OnWindowResize(NIB_UNUSED WindowResizeEvent& event);

	void	DrawSquare(int8_t x, int8_t y, const vec4& color, bool filled = true);

private:
	ClientGame _game;
	RendererAPI::API _request_new_api;

	NibblerSocket _socket;
	pollfd* _poll_fds;
	NibblerNfds_t _nfds;
	uint8_t _id;
	Direction _requested_direction;

	uint32_t _fps;
	bool _local_multiplayer;
	bool _show_debug;
};

} // Nibbler

#include "Nibbler/Client/Client.hpp"
#include "Nibbler/Server/Server.hpp"
#include "Nibbler/Renderer/RenderCommand.hpp"

namespace Nibbler
{

bool	g_ApplicationRunning = true;

Client::Client(const ClientConfig& config)
	: _game(),
	  _socket(INVALID_SOCKET),
	  _poll_fds(nullptr),
	  _nfds(1),
	  _id(-1),
	  _requested_direction(Direction::Right)
{
	Connect(config.host, config.port);

	std::vector<SquareType> hvec;
	hvec.resize(_game.height, SquareType::Empty);
	for (int x = 0; x < _game.width; x++)
		_game.squares.push_back(hvec);

	InitRenderer(config.api);
}

Client::~Client()
{
	Close();
	Disconnect();

	Renderer::Shutdown();
}

void	Client::Update()
{
	// Receive game state
	ReceivePacket();
}

void	Client::Render()
{
	RenderCommand::SetClearColor(COLOR_BG);
	RenderCommand::Clear();

	for (int8_t x = 0; x < _game.width; x++)
	{
		for (int8_t y = 0; y < _game.height; y++)
		{
			switch (_game.squares[x][y])
			{
				case SquareType::Empty:
					if ((x % 2 != 0 && y % 2 == 0) || (x % 2 == 0 && (y + 1) % 2 == 0))
						DrawSquare(x, y, COLOR_BOARD_1);
					else
						DrawSquare(x, y, COLOR_BOARD_2);
					break;
				case SquareType::Apple:      DrawSquare(x, y, COLOR_APPLE); break;
				case SquareType::Snake0:     DrawSquare(x, y, COLOR_SNAKE0_BODY); break;
				case SquareType::Snake0Head: DrawSquare(x, y, COLOR_SNAKE0_HEAD); break;
				case SquareType::Snake1:     DrawSquare(x, y, COLOR_SNAKE1_BODY); break;
				case SquareType::Snake1Head: DrawSquare(x, y, COLOR_SNAKE1_HEAD); break;
				case SquareType::Snake2:     DrawSquare(x, y, COLOR_SNAKE2_BODY); break;
				case SquareType::Snake2Head: DrawSquare(x, y, COLOR_SNAKE2_HEAD); break;
				case SquareType::Snake3:     DrawSquare(x, y, COLOR_SNAKE3_BODY); break;
				case SquareType::Snake3Head: DrawSquare(x, y, COLOR_SNAKE3_HEAD); break;
			}
		}
	}
}

void	Client::Run()
{
	_request_new_api = Renderer::GetCurrentAPI();

	auto lastFrameTime = Clock::now();
	auto lastSecondTime = lastFrameTime;

	int frames = 0, fps = 0;

	while (g_ApplicationRunning)
	{
		// Poll events
		Renderer::BeginFrame();

		// API might have changed
		if (_request_new_api != Renderer::GetCurrentAPI())
		{
			InitRenderer(_request_new_api);
			_request_new_api = Renderer::GetCurrentAPI();
			continue;
		}

		auto currentTime = Clock::now();
		lastFrameTime = currentTime;

		if (std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - lastSecondTime).count() >= NS_PER_SECOND)
		{
			fps = frames;
			frames = 0;
			lastSecondTime = currentTime;
		}
		
		Update();
		Render();
		frames++;

		Renderer::BeginImGui();
		
		ImGui::SetNextWindowBgAlpha(0.7f); // Transparent background
		ImGui::Begin("Nibbler Client");
		ImGui::SeparatorText("Client");
		ImGui::Text("ID: %d", _id);
		ImGui::Text("FPS: %d", fps);
		// ImGui::Text("Current API:");
        ImGui::RadioButton("SDL3",    (int *)&_request_new_api, RendererAPI::API::SDL3); ImGui::SameLine();
        ImGui::RadioButton("GLFW",    (int *)&_request_new_api, RendererAPI::API::GLFW); ImGui::SameLine();
        ImGui::RadioButton("Allegro", (int *)&_request_new_api, RendererAPI::API::Allegro);

		if (ImGui::Button("Add player"))
			SendEnableMultiplayerPacket();
		ImGui::SameLine();
		if (ImGui::Button("Exit"))
			Close();
		ImGui::End();

		if (Server::Running())
			Server::RenderImGuiDebug();
		
		// ImGui::ShowDemoWindow();
		Renderer::EndImGui();
		Renderer::EndFrame();

		// Cap framerate
		if (!Renderer::GetVSync())
		{
			auto frameEndTime = Clock::now();
			auto frameDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(frameEndTime - lastFrameTime).count();
			if (frameDuration < NS_PER_FRAME)
			{
				auto sleepDuration = std::chrono::nanoseconds(NS_PER_FRAME - frameDuration);
				std::this_thread::sleep_for(sleepDuration);
			}
		}
	}
}

void	Client::Close()
{
	g_ApplicationRunning = false;
	if (Server::Running())
		Server::Stop();
}

void	Client::InitRenderer(RendererAPI::API api)
{
	Renderer::Init(api,
		RendererAPIConfig{
			.vsync = true,
			.event_callback = NIB_BIND_EVENT_FN(Client::OnEvent),
		}
	);
	}

bool	Client::OnKeyPressed(KeyPressedEvent& event)
{
	switch (event.GetKeyCode())
	{
		case Key::F1:
			_request_new_api = RendererAPI::API::SDL3;
			break;
		case Key::F2:
			_request_new_api = RendererAPI::API::GLFW;
			break;
		case Key::F3:
			_request_new_api = RendererAPI::API::Allegro;
			break;
		case Key::Escape:
			Close();
			break;
		case Key::Space:
			SendInputPacket(ClientInput::Pause);
			break;
		case Key::Left:
			SendInputPacket(ClientInput::Left);
			break;
		case Key::Right:
			SendInputPacket(ClientInput::Right);
			break;
		case Key::Up:
			SendInputPacket(ClientInput::Up);
			break;
		case Key::Down:
			SendInputPacket(ClientInput::Down);
			break;

		// Local multiplayer
		case Key::A:
			SendInputPacket(ClientInput::A);
			break;
		case Key::D:
			SendInputPacket(ClientInput::D);
			break;
		case Key::W:
			SendInputPacket(ClientInput::W);
			break;
		case Key::S:
			SendInputPacket(ClientInput::S);
			break;
		default:
			break;
	}
	return true;
}

bool	Client::OnWindowClose(WindowCloseEvent& event)
{
	NIB_NOTUSED(event);

	Close();
	return true;
}

bool	Client::OnWindowResize(NIB_UNUSED WindowResizeEvent& event)
{
	NIB_NOTUSED(event);

	if (event.GetWidth() == 0 || event.GetHeight() == 0)
	{
		// _minimized = true;
		return false;
	}

	// _minimized = false;
	ivec2 size = Renderer::GetWindowSize();
	Renderer::OnWindowResize(size.x, size.y);

	// Forward the event
	return false;
}

bool	Client::OnEvent(Event& event)
{
	EventDispatcher	dispatcher(event);

	dispatcher.Dispatch<KeyPressedEvent>(NIB_BIND_EVENT_FN(OnKeyPressed));
	dispatcher.Dispatch<WindowCloseEvent>(NIB_BIND_EVENT_FN(OnWindowClose));
	dispatcher.Dispatch<WindowResizeEvent>(NIB_BIND_EVENT_FN(OnWindowResize));
	return true;
}

void	Client::DrawSquare(int8_t x, int8_t y, const vec4& color, bool filled)
{
	NIB_ASSERT(x >= 0 && x < _game.width && y >= 0 && y < _game.height, "DrawSquare(): tried to draw out of bounds ({}, {})", x, y);
	NIB_ASSERT(_game.width > 0 && _game.height > 0);

	ivec2 winsize = Renderer::GetWindowSize();

	NIB_ASSERT(winsize.x > 0 && winsize.y > 0);

	Rect rect = {
		0.0f,
		0.0f,
		(winsize.x / (float)_game.width / winsize.x) * 0.9f,
		(winsize.y / (float)_game.height / winsize.y) * 0.9f,
	};

	if (rect.w < rect.h)
		rect.h = rect.w;
	else
		rect.w = rect.h;

	rect.x = (x * rect.w) + 0.05f;
	rect.y = (y * rect.h) + 0.05f;

	if (filled)
		RenderCommand::FillRect(rect, color);
	else
		RenderCommand::DrawRect(rect, color);
}

} // Nibbler

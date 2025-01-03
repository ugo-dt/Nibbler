#include "SDL3RendererAPI.hpp"

namespace Nibbler
{

NIBBLER_API RendererAPI* NIBBLERCALL	Nibbler_CreateRenderAPI(const RendererAPIConfig& config)
{
	return new SDL3RendererAPI(config);
}

SDL3RendererAPI::SDL3RendererAPI(const RendererAPIConfig& config)
{
	Log::Init();
#ifdef NIB_RELEASE
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
#else
	bool status = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	NIB_ASSERT(status == true && "Failed to init SDL");
	Log::Info("Initialized SDL.");
#endif

	_window = SDL_CreateWindow(config.title, config.width, config.height, 0);
	NIB_ASSERT(_window != nullptr, "SDL_CreateWindow(): {}", SDL_GetError());
	_renderer = SDL_CreateRenderer(_window, nullptr);
	NIB_ASSERT(_renderer != nullptr, "SDL_CreateRenderer(): {}", SDL_GetError());
	SDL_SetRenderVSync(_renderer, config.vsync);

	SDL_SetRenderLogicalPresentation(_renderer, config.width, config.height, SDL_LOGICAL_PRESENTATION_LETTERBOX);

	_event_callback = config.event_callback;

	Log::Info("Created SDL Window '{}' ({}, {}).", config.title, config.width, config.height);

	// ImGui
	IMGUI_CHECKVERSION();
	_imgui_ctx =  ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	// Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;		// Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;		// Enable Multi-Viewport / Platform Windows
    ImGui::StyleColorsDark();

    ImGui_ImplSDL3_InitForSDLRenderer(_window, _renderer);
    ImGui_ImplSDLRenderer3_Init(_renderer);
}

SDL3RendererAPI::~SDL3RendererAPI()
{
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

	if (_renderer)
		SDL_DestroyRenderer(_renderer);
	if (_window)
	{
		SDL_DestroyWindow(_window);
		Log::Info("Closed SDL window.");
	}
	SDL_Quit();
}

vec4	SDL3RendererAPI::CapNormalizedVec(const vec4& v)
{
	return vec4(
		v.x > 1.0f ? 1.0f : v.x,
		v.y > 1.0f ? 1.0f : v.y,
		v.z > 1.0f ? 1.0f : v.b,
		v.w > 1.0f ? 1.0f : v.w
	);
}

void	SDL3RendererAPI::HandleKeyEvents(const SDL_KeyboardEvent& keyboard_event)
{
	if (keyboard_event.down)
	{
		KeyPressedEvent event(keyboard_event.scancode, keyboard_event.repeat);
		_event_callback(event);
	}
	else
	{
		KeyReleasedEvent event(keyboard_event.scancode);
		_event_callback(event);
	}
}

void	SDL3RendererAPI::HandleWindowEvents(const SDL_WindowEvent& window_event)
{
	switch (window_event.type)
	{
		case SDL_EVENT_WINDOW_RESIZED:
		{
			WindowResizeEvent event(window_event.data1, window_event.data2);
			_event_callback(event);
			break;
		}
		default:
			break;
	}
}

void	SDL3RendererAPI::SetWindowTitle(const std::string& title)
{
	SDL_SetWindowTitle(_window, title.c_str());
}

void	SDL3RendererAPI::BeginFrame()
{
	SDL_Event	event;

	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL3_ProcessEvent(&event);
		switch (event.type)
		{
		case SDL_EVENT_WINDOW_CLOSE_REQUESTED: // event.window.windowID == SDL_GetWindowID(window)
		case SDL_EVENT_QUIT:
		{
			WindowCloseEvent event;
			_event_callback(event);
			break;
		}
		case SDL_EVENT_KEY_DOWN:
		case SDL_EVENT_KEY_UP:
			HandleKeyEvents(event.key);
			break;
		// case SDL_EVENT_MOUSE_BUTTON_DOWN:
		// case SDL_EVENT_MOUSE_BUTTON_UP:
		// 	HandleMouseButtonEvents(event.button);
		// 	break;
		// case SDL_EVENT_MOUSE_WHEEL:
		// 	HandleMouseWheelEvents(event.wheel.x, event.wheel.y);
		// 	break;
		// case SDL_EVENT_MOUSE_MOTION:
		// 	HandleMouseMotionEvents(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
		// 	break;
		default:
			if (event.type >= SDL_EVENT_WINDOW_FIRST && event.type <= SDL_EVENT_WINDOW_LAST)
				HandleWindowEvents(event.window);
			break;
		}
	}
}

void	SDL3RendererAPI::EndFrame()
{
	SDL_RenderPresent(_renderer);
}

void	SDL3RendererAPI::BeginImGui()
{
	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();
}

void	SDL3RendererAPI::EndImGui()
{
	ImGui::Render();
	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), _renderer);
}

ImGuiContext*	SDL3RendererAPI::GetImGuiContext()
{
	return _imgui_ctx;
}

bool	SDL3RendererAPI::GetVSync() const
{
	int vsync;
	SDL_GetRenderVSync(_renderer, &vsync);
	return vsync;
}

ivec2	SDL3RendererAPI::GetWindowSize() const
{
	int w, h;

	SDL_GetWindowSizeInPixels(_window, &w, &h);
	return ivec2{w, h};
}

bool	SDL3RendererAPI::IsKeyPressed(KeyCode key)
{
	return SDL_GetKeyboardState(nullptr)[key];
}

bool	SDL3RendererAPI::IsMouseButtonPressed(MouseButton button)
{
	return SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON_MASK(button);
}

vec2	SDL3RendererAPI::GetMousePosition() const
{
	float xpos, ypos;

	SDL_GetMouseState(&xpos, &ypos);
	return vec2{xpos, ypos};
}

float	SDL3RendererAPI::GetMouseX() const
{
	return GetMousePosition().x;
}

float	SDL3RendererAPI::GetMouseY() const
{
	return GetMousePosition().y;
}

void	SDL3RendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	NIB_NOTUSED(x);
	NIB_NOTUSED(y);
	NIB_NOTUSED(width);
	NIB_NOTUSED(height);

	/* We use letterboxing with SDL_SetRenderLogicalPresentation(), viewport is updated automatically */
}

void	SDL3RendererAPI::SetClearColor(const vec4& color)
{
	vec4 _color = CapNormalizedVec(color);

	SDL_SetRenderDrawColor(
		_renderer,
		_color.r * 0xff,
		_color.g * 0xff,
		_color.b * 0xff,
		_color.a * 0xff
	);
}

void	SDL3RendererAPI::Clear()
{
	SDL_RenderClear(_renderer);
}

void	SDL3RendererAPI::DrawRect(const Rect& rect, const vec4& color)
{
	SDL_Rect _vp;
	SDL_GetRenderViewport(_renderer, &_vp);

	Rect _rect = CapNormalizedVec(rect);

	SDL_FRect frect = {
		.x = _rect.x * _vp.w,
		.y = _rect.y * _vp.h,
		.w = _rect.w * _vp.w,
		.h = _rect.h * _vp.h,
	};

	SetClearColor(color);
	SDL_RenderRect(_renderer, &frect);
}

void	SDL3RendererAPI::FillRect(const Rect& rect, const vec4& color)
{
	SDL_Rect _vp;
	SDL_GetRenderViewport(_renderer, &_vp);

	Rect _rect = CapNormalizedVec(rect);

	SDL_FRect frect = {
		.x = _rect.x * _vp.w,
		.y = _rect.y * _vp.h,
		.w = _rect.w * _vp.w,
		.h = _rect.h * _vp.h,
	};

	SetClearColor(color);
	SDL_RenderFillRect(_renderer, &frect);
}

} // Nibbler

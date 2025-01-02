#include "AllegroRendererAPI.hpp"

namespace Nibbler
{

KeyCode ALLEGROKeyToKeyCode(int key);
int KeyCodeToALLEGROKey(KeyCode key);

NIBBLER_API RendererAPI* NIBBLERCALL	Nibbler_CreateRenderAPI(const RendererAPIConfig& config)
{
	return new AllegroRendererAPI(config);
}

AllegroRendererAPI::AllegroRendererAPI(const RendererAPIConfig& config)
	: _event_callback(config.event_callback),
	  _winsize(config.width, config.height)
{
    Log::Init();
#ifdef NIB_RELEASE
	al_install_system(ALLEGRO_VERSION_INT, nullptr);
#else
	bool status = al_install_system(ALLEGRO_VERSION_INT, nullptr);
	NIB_ASSERT(status == true, "al_install_system()");
	Log::Info("Initialized Allegro.");
#endif

	al_init_primitives_addon();
	al_install_keyboard();
    al_install_mouse();

	al_set_new_window_position(INT_MAX, INT_MAX);
	al_set_new_display_option(ALLEGRO_VSYNC, config.vsync, ALLEGRO_SUGGEST); // vsync 1 means forced on, 2 means forced off.
	_display = al_create_display(config.width, config.height);
	NIB_ASSERT(_display != nullptr, "al_create_display()");

	_queue = al_create_event_queue();
	al_register_event_source(_queue, al_get_display_event_source(_display));
	al_register_event_source(_queue, al_get_keyboard_event_source());
	al_register_event_source(_queue, al_get_mouse_event_source());

	_clear_color = al_map_rgb(0, 0, 0);

	Log::Info("Created Allegro Window '{}' ({}, {}).", config.title, config.width, config.height);

	// ImGui
	IMGUI_CHECKVERSION();
	_imgui_ctx =  ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	// Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;		// Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;		// Enable Multi-Viewport / Platform Windows
    ImGui::StyleColorsDark();

    ImGui_ImplAllegro5_Init(_display);
}

AllegroRendererAPI::~AllegroRendererAPI()
{
    ImGui_ImplAllegro5_Shutdown();
    ImGui::DestroyContext();

	al_destroy_event_queue(_queue);
	al_uninstall_mouse();
	al_uninstall_keyboard();
	al_shutdown_primitives_addon();
	al_destroy_display(_display);
	al_uninstall_system();

	Log::Info("Closed Allegro window.");
}

vec4 AllegroRendererAPI::CapNormalizedVec(const vec4& v)
{
	return vec4(
		v.x > 1.0f ? 1.0f : v.x,
		v.y > 1.0f ? 1.0f : v.y,
		v.z > 1.0f ? 1.0f : v.z,
		v.w > 1.0f ? 1.0f : v.w
	);
}

void	AllegroRendererAPI::HandleKeyEvents(const ALLEGRO_KEYBOARD_EVENT& keyboard_event, bool down)
{
	//FIXME: keyboards layouts
	// A on azerty sends A. it should send Q.

	KeyCode nibblerkey = ALLEGROKeyToKeyCode(keyboard_event.keycode);
	if (down)
	{
		KeyPressedEvent event(nibblerkey, keyboard_event.repeat);
		_event_callback(event);
	}
	else
	{
		KeyReleasedEvent event(nibblerkey);
		_event_callback(event);
	}
}

void	AllegroRendererAPI::HandleWindowEvents(const ALLEGRO_DISPLAY_EVENT& display_event)
{
	switch (display_event.type)
	{
		case ALLEGRO_EVENT_DISPLAY_RESIZE:
		{
			WindowResizeEvent event(display_event.width, display_event.height);
			_event_callback(event);
			ImGui_ImplAllegro5_InvalidateDeviceObjects();
			al_acknowledge_resize(display_event.source);
			ImGui_ImplAllegro5_CreateDeviceObjects();
			break;
		}
		default:
			break;
	}
}

void	AllegroRendererAPI::SetWindowTitle(const std::string& title)
{
	al_set_window_title(_display, title.c_str());
}

void	AllegroRendererAPI::BeginFrame()
{
	ALLEGRO_EVENT	event;

	if (!al_is_event_queue_empty(_queue))
	{
		while (al_get_next_event(_queue, &event))
		{
            ImGui_ImplAllegro5_ProcessEvent(&event);
			switch (event.type)
			{
				case ALLEGRO_EVENT_DISPLAY_CLOSE:
				{
					WindowCloseEvent event;
					_event_callback(event);
					break;
				}
				case ALLEGRO_EVENT_KEY_DOWN:
					HandleKeyEvents(event.keyboard, true);
					break;
				case ALLEGRO_EVENT_KEY_UP:
					HandleKeyEvents(event.keyboard, false);
					break;
				default:
					if (event.type >= ALLEGRO_EVENT_DISPLAY_EXPOSE && event.type <= ALLEGRO_EVENT_DISPLAY_RESUME_DRAWING)
						HandleWindowEvents(event.display);
					break;
			}
		}
	}
}

void	AllegroRendererAPI::EndFrame()
{
	al_flip_display();
}

void	AllegroRendererAPI::BeginImGui()
{
	ImGui_ImplAllegro5_NewFrame();
	ImGui::NewFrame();
}

void	AllegroRendererAPI::EndImGui()
{
	ImGui::Render();
	ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData());
}

ImGuiContext*	AllegroRendererAPI::GetImGuiContext()
{
	return _imgui_ctx;
}

bool	AllegroRendererAPI::GetVSync() const
{
	return true;
}

ivec2 AllegroRendererAPI::GetWindowSize() const
{
    return ivec2{al_get_display_width(_display), al_get_display_height(_display)};
}

bool	AllegroRendererAPI::IsKeyPressed(KeyCode key)
{
	ALLEGRO_KEYBOARD_STATE kbdstate;
	al_get_keyboard_state(&kbdstate);
	return al_key_down(&kbdstate, KeyCodeToALLEGROKey(key));
}

bool	AllegroRendererAPI::IsMouseButtonPressed(MouseButton button)
{
	ALLEGRO_MOUSE_STATE mousestate;
	al_get_mouse_state(&mousestate);
	return al_mouse_button_down(&mousestate, button + 1);
}

vec2	AllegroRendererAPI::GetMousePosition() const
{
	int x, y;
	al_get_mouse_cursor_position(&x, &y);
    return vec2{static_cast<float>(x), static_cast<float>(y)};
}

float	AllegroRendererAPI::GetMouseX() const
{
    return GetMousePosition().x;
}

float	AllegroRendererAPI::GetMouseY() const
{
    return GetMousePosition().y;
}

void	AllegroRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	NIB_NOTUSED(x);
	NIB_NOTUSED(y);
	NIB_NOTUSED(width);
	NIB_NOTUSED(height);
}

void	AllegroRendererAPI::SetClearColor(const vec4& color)
{
	vec4 _color = CapNormalizedVec(color);
	_clear_color = al_map_rgba(_color.r * 0xff, _color.g * 0xff, _color.b * 0xff, _color.a * 0xff);
}

void	AllegroRendererAPI::Clear()
{
	al_clear_to_color(_clear_color);
}

void	AllegroRendererAPI::DrawRect(const Rect& rect, const vec4& color)
{
	Rect _rect = CapNormalizedVec(rect);

	float _x = _rect.x * _winsize.x;
	float _y = _rect.y * _winsize.y;
	float _w = _rect.w * _winsize.x;
	float _h = _rect.h * _winsize.y;

	SetClearColor(color);
	al_draw_rectangle(_x, _y, _x + _w, _y + _h, _clear_color, 0);
}

void	AllegroRendererAPI::FillRect(const Rect& rect, const vec4& color)
{
	Rect _rect = CapNormalizedVec(rect);

	float _x = _rect.x * _winsize.x;
	float _y = _rect.y * _winsize.y;
	float _w = _rect.w * _winsize.x;
	float _h = _rect.h * _winsize.y;

	SetClearColor(color);
	al_draw_filled_rectangle(_x, _y, _x + _w, _y + _h, _clear_color);
}

} // Nibbler

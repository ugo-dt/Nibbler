#include "glfwRendererAPI.hpp"

namespace Nibbler
{

extern EventCallback*	event_callback_ptr;

void	KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void	WindowSizeCallback(GLFWwindow* window, int width, int height);
void	WindowCloseCallback(GLFWwindow* window);
int		KeyCodeToGLFWKey(KeyCode key);

NIBBLER_API RendererAPI* NIBBLERCALL	Nibbler_CreateRenderAPI(const RendererAPIConfig& config)
{
	return new glfwRendererAPI(config);
}

glfwRendererAPI::glfwRendererAPI(const RendererAPIConfig& config)
{
	Log::Init();
	bool status = glfwInit();
	if (status == false)
		Log::Critical("Failed to initialize GLFW!");
	Log::Info("Initialized GLFW.");

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_SCALE_FRAMEBUFFER, GLFW_TRUE);

	_window = glfwCreateWindow(config.width, config.height, config.title, nullptr, nullptr);
	if (_window == nullptr)
		Log::Critical("Could not create the GLFW window.");

	float xscale, yscale;
	glfwGetWindowContentScale(_window, &xscale, &yscale);

	if (xscale != 1 || yscale != 1)
		glfwSetWindowSize(_window, config.width / xscale, config.height / yscale);

	glfwMakeContextCurrent(_window);
	if (config.vsync)
	{
		glfwSwapInterval(1); // Enable VSync
	}

	int framebufferWidth, framebufferHeight;
	glfwGetFramebufferSize(_window, &framebufferWidth, &framebufferHeight);
	glViewport(0, 0, framebufferWidth, framebufferHeight);

	glOrtho(-1.0, 1.0, 1.0, -1.0, -1.0, 1.0);

	_event_callback = config.event_callback;
	event_callback_ptr = &_event_callback;

	glfwSetKeyCallback(_window, KeyCallback);
	glfwSetWindowSizeCallback(_window, WindowSizeCallback);
	glfwSetWindowCloseCallback(_window, WindowCloseCallback);

	Log::Info("Created GLFW Window '{}' ({}, {}).", config.title, config.width, config.height);

	// ImGui
	IMGUI_CHECKVERSION();
	_imgui_ctx =  ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	// Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;		// Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;		// Enable Multi-Viewport / Platform Windows
	ImGui::StyleColorsDark();
	
	ImGui_ImplGlfw_InitForOpenGL(_window, true);
	ImGui_ImplOpenGL2_Init();
}

glfwRendererAPI::~glfwRendererAPI()
{
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	if (_window)
	{
		glfwDestroyWindow(_window);
		Log::Info("Closed GLFW window.");
	}
	glfwTerminate();
}

vec4 glfwRendererAPI::CapNormalizedVec(const vec4& v)
{
	return vec4(
		v.x > 1.0f ? 1.0f : v.x,
		v.y > 1.0f ? 1.0f : v.y,
		v.z > 1.0f ? 1.0f : v.z,
		v.w > 1.0f ? 1.0f : v.w
	);
}

void	glfwRendererAPI::SetWindowTitle(const std::string& title)
{
	glfwSetWindowTitle(_window, title.c_str());
}

void	glfwRendererAPI::BeginFrame()
{
	glfwPollEvents();
	
	if (glfwWindowShouldClose(_window))
	{
		WindowCloseEvent event;
		_event_callback(event);
	}
}

void	glfwRendererAPI::EndFrame()
{
	glfwMakeContextCurrent(_window);
	glfwSwapBuffers(_window);
}

void	glfwRendererAPI::BeginImGui()
{
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void	glfwRendererAPI::EndImGui()
{
	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

ImGuiContext*	glfwRendererAPI::GetImGuiContext()
{
	return _imgui_ctx;
}

bool	glfwRendererAPI::GetVSync() const
{
	return glfwGetWindowAttrib(_window, GLFW_DOUBLEBUFFER) != 0;
}

ivec2 glfwRendererAPI::GetWindowSize() const
{
	int width, height;
	glfwGetWindowSize(_window, &width, &height);
	return ivec2{width, height};
}

bool	glfwRendererAPI::IsKeyPressed(KeyCode key)
{
	return glfwGetKey(_window, KeyCodeToGLFWKey(key)) == GLFW_PRESS;
}

bool	glfwRendererAPI::IsMouseButtonPressed(MouseButton button)
{
	//TODO: switch statement for mouse
	return glfwGetMouseButton(_window, button - 1) == GLFW_PRESS;
}

vec2	glfwRendererAPI::GetMousePosition() const
{
	double xpos, ypos;
	glfwGetCursorPos(_window, &xpos, &ypos);
	return vec2{static_cast<float>(xpos), static_cast<float>(ypos)};
}

float	glfwRendererAPI::GetMouseX() const
{
	return GetMousePosition().x;
}

float	glfwRendererAPI::GetMouseY() const
{
	return GetMousePosition().y;
}

void	glfwRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	glViewport(x, y, width, height);
}

void	glfwRendererAPI::SetClearColor(const vec4& color)
{
	vec4 _color = CapNormalizedVec(color);
	glClearColor(_color.r, _color.g, _color.b, _color.a);
}

void	glfwRendererAPI::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void	glfwRendererAPI::DrawRect(const Rect& rect, const vec4& color)
{
	Rect _rect = CapNormalizedVec(rect);

	_rect.x = _rect.x * 2.f - 1.f;
	_rect.y = _rect.y * 2.f - 1.f;
	_rect.w = _rect.w * 2.f;
	_rect.h = _rect.h * 2.f;

	glColor4f(color.r, color.g, color.b, color.a);
	glBegin(GL_LINE_LOOP);
	glVertex2f(_rect.x, _rect.y);
	glVertex2f(_rect.x + _rect.w, _rect.y);
	glVertex2f(_rect.x + _rect.w, _rect.y + _rect.h);
	glVertex2f(_rect.x, _rect.y + _rect.h);
	glEnd();
}

void	glfwRendererAPI::FillRect(const Rect& rect, const vec4& color)
{
	Rect _rect = CapNormalizedVec(rect);

	_rect.x = _rect.x * 2.f - 1.f;
	_rect.y = _rect.y * 2.f - 1.f;
	_rect.w = _rect.w * 2.f;
	_rect.h = _rect.h * 2.f;

	glColor4f(color.r, color.g, color.b, color.a);
	glBegin(GL_QUADS);
	glVertex2f(_rect.x, _rect.y);
	glVertex2f(_rect.x + _rect.w, _rect.y);
	glVertex2f(_rect.x + _rect.w, _rect.y + _rect.h);
	glVertex2f(_rect.x, _rect.y + _rect.h);
	glEnd();
}

} // Nibbler

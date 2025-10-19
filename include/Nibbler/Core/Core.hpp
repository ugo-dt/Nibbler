#pragma once

#include <cassert>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <unistd.h>

#include <chrono>
#include <deque>
#include <filesystem>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <random>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#ifdef _WIN32
	#define WIN32_MEAN_AND_LEAN
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <ws2ipdef.h>
	#include <windows.h>
	#include <direct.h>
#else
	#include <arpa/inet.h>
	#include <dlfcn.h>
	#include <fcntl.h>
	#include <net/if.h>
	#include <netinet/ip.h>
	#include <netinet/in.h>
	#include <sys/ioctl.h>
	#include <sys/poll.h>
	#include <sys/socket.h>
	#include <sys/types.h>
#endif

#include <imgui/imgui.h>

#if defined(_WIN32)
#  define NIBBLER_API extern "C" __declspec(dllexport)
#  define NIBBLERCALL __cdecl
#  define NIBBLER_DYNAMIC_LIB_SDL "NibblerSDL/NibblerSDL.dll"
#  define NIBBLER_DYNAMIC_LIB_GLFW "NibblerGLFW/NibblerGLFW.dll"
#  define NIBBLER_DYNAMIC_LIB_ALLEGRO "NibblerAllegro/NibblerAllegro.dll"
# else
#  define NIBBLER_API extern "C"
#  define NIBBLERCALL
#  define NIBBLER_DYNAMIC_LIB_SDL "NibblerSDL/NibblerSDL.so"
#  define NIBBLER_DYNAMIC_LIB_GLFW "NibblerGLFW/NibblerGLFW.so"
#  define NIBBLER_DYNAMIC_LIB_ALLEGRO "NibblerAllegro/NibblerAllegro.so"
#endif

#define BIT(x) (1 << x)
#define NIB_EXPAND_MACRO(x) x
#define NIB_STRINGIFY(x) #x
#define NIB_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#if defined(NIB_DEBUG) || defined(NIB_DEV)
	#define NIB_ENABLE_ASSERTS
#endif

#if defined(__clang__) || defined(__GNUC__)
	#define NIB_INLINE		inline __attribute__((__always_inline__))
	#define NIB_NEVER_INLINE	__attribute__((__noinline__))
	#define NIB_NORETURN		__attribute__ ((noreturn))
	#define NIB_UNUSED		__attribute__ ((unused))
	#define NIB_NOTUSED(x)	((void)(x));
	#define NIB_NODISCARD	[[nodiscard]]
#else
	#define NIB_INLINE		inline
	#define NIB_NEVER_INLINE
	#define NIB_NORETURN
	#define NIB_UNUSED
	#define NIB_NOTUSED(x)
	#define NIB_NODISCARD
#endif // defined(__clang__) || defined(__GNUC__)

namespace Nibbler
{

extern bool g_ApplicationRunning;

template<typename T>
using Scope = std::unique_ptr<T>;
template<typename T, typename ... Args>
constexpr Scope<T> CreateScope(Args&& ... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using Ref = std::shared_ptr<T>;
template<typename T, typename ... Args>
constexpr Ref<T> CreateRef(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

} // Nibbler

#include "Assert.hpp"
#include "Log.hpp"
#include "Vec.hpp"

namespace Nibbler
{

constexpr int	FPS_CAP = 60;
constexpr int	TICK_RATE = 10;
constexpr auto	NS_PER_SECOND = 1'000'000'000LL;
constexpr auto	NS_PER_FRAME = NS_PER_SECOND / FPS_CAP;
constexpr auto	NS_PER_TICK = NS_PER_SECOND / TICK_RATE;

constexpr vec4	COLOR_BLACK = vec4(0.0f, 0.0f, 0.0f, 1.0f);
constexpr vec4	COLOR_WHITE = vec4(1.0f, 1.0f, 1.0f, 1.0f);
constexpr vec4	COLOR_BOARD_1 = vec4(0.67f, 0.84f, 0.32f, 1.0f);
constexpr vec4	COLOR_BOARD_2 = vec4(0.64f, 0.78f, 0.29f, 1.0f);
constexpr vec4	COLOR_BG = vec4(0.34f, 0.54f, 0.2f, 1.0f);
constexpr vec4	COLOR_APPLE = vec4(1.0f, 0.34f, 0.2f, 1.0f);
constexpr vec4	COLOR_SNAKE0_HEAD = vec4(0.24f, 0.14f, 0.6f, 1.0f);
constexpr vec4	COLOR_SNAKE0_BODY = vec4(0.34f, 0.24f, 0.7f, 1.0f);
constexpr vec4	COLOR_SNAKE1_HEAD = vec4(0.14f, 0.8f, 0.75f, 1.0f);
constexpr vec4	COLOR_SNAKE1_BODY = vec4(0.24f, 0.9f, 0.85f, 1.0f);
constexpr vec4	COLOR_SNAKE2_HEAD = vec4(0.85f, 0.85f, 0.85f, 1.0f);
constexpr vec4	COLOR_SNAKE2_BODY = vec4(1.0f, 1.0f, 1.0f, 1.0f);
constexpr vec4	COLOR_SNAKE3_HEAD = vec4(0.9f, 0.0f, 0.85f, 1.0f);
constexpr vec4	COLOR_SNAKE3_BODY = vec4(1.0f, 0.0f, 0.95f, 1.0f);

constexpr uint32_t	MAX_CLIENTS = 2;
constexpr uint8_t	SNAKE_DEFAULT_SIZE = 4;

using Clock = std::chrono::high_resolution_clock;

#ifdef _WIN32
using NibblerSocket = SOCKET;
using NibblerNfds_t = ULONG;
#else
using NibblerSocket = int;
using NibblerNfds_t = nfds_t;
#define INVALID_SOCKET	((NibblerSocket)-1)
#endif

template <typename T, int MaxLen = 1, typename Container=std::deque<T>>
class FixedQueue : public std::queue<T, Container>
{
public:
	void push(const T& value)
	{
		if (this->size() == MaxLen)
			this->c.pop_front();
		std::queue<T, Container>::push(value);
	}
};

} // Nibbler

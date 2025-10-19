#pragma once

#include "Nibbler/Core/Core.hpp"
#include <format>
#include <unistd.h>

namespace Nibbler
{

static constexpr char LOG_COLOR_DEFAULT[]	= "\033[39m";
static constexpr char LOG_COLOR_RED[]		= "\033[91m";
static constexpr char LOG_COLOR_GREEN[]		= "\033[92m";
static constexpr char LOG_COLOR_YELLOW[]	= "\033[93m";
static constexpr char LOG_COLOR_BLUE[]		= "\033[94m";
static constexpr char LOG_COLOR_MAGENTA[]	= "\033[95m";
static constexpr char LOG_COLOR_CYAN[]		= "\033[96m";
static constexpr char LOG_COLOR_WHITE[]		= "\033[97m";

class Log
{
public:
	static void	Init();

	template <class... Args>
	static void	Trace(std::string_view fmt, Args&&... args);

	template <class... Args>
	static void	Info(std::string_view fmt, Args&&... args);

	template <class... Args>
	static void	Warn(std::string_view fmt, Args&&... args);

	template <class... Args>
	static void	Error(std::string_view fmt, Args&&... args);

	template <class... Args>
	static void	Critical(std::string_view fmt, Args&&... args);

private:
	template <typename OStream>
	static void	Print(OStream& stream, const char *color, const char *log_name, std::string_view fmt, std::format_args args, const std::string_view& end = "\n");

private:
	static inline std::vector<std::ostream*>	_output_streams;
	static inline std::vector<std::ostream*>	_error_streams;
	static inline std::mutex					_mutex;
};

template <typename OStream>
inline void
Log::Print(OStream& stream, const char *color, const char *log_name, std::string_view fmt, std::format_args args, const std::string_view& end)
{
	if (fmt.empty())
		return;

	_mutex.lock();
	bool tty = isatty(STDOUT_FILENO);

	if (tty)
		stream << color;
	stream << "[" << log_name << "] ";
	if (tty)
		stream << LOG_COLOR_DEFAULT;
	stream << std::vformat(fmt, args) << end;
	_mutex.unlock();
}

template <class... Args>
inline void
Log::Trace(std::string_view fmt, Args&&... args)
{
	for (auto stream : _output_streams)
		Log::Print(*stream, LOG_COLOR_BLUE, "TRACE", fmt, std::make_format_args(args...));
}

template <class... Args>
inline void
Log::Info(std::string_view fmt, Args&&... args)
{
	for (auto stream : _output_streams)
		Log::Print(*stream, LOG_COLOR_WHITE, "INFO", fmt, std::make_format_args(args...));
}

template <class... Args>
inline void
Log::Warn(std::string_view fmt, Args&&... args)
{
	for (auto stream : _error_streams)
		Log::Print(*stream, LOG_COLOR_YELLOW, "WARN", fmt, std::make_format_args(args...));
}

template <class... Args>
inline void
Log::Error(std::string_view fmt, Args&&... args)
{
	for (auto stream : _error_streams)
		Log::Print(*stream, LOG_COLOR_RED, "ERROR", fmt, std::make_format_args(args...));
}

template <class... Args>
inline void
Log::Critical(std::string_view fmt, Args&&... args)
{
	for (auto stream : _error_streams)
		Log::Print(*stream, LOG_COLOR_RED, "CRITICAL", fmt, std::make_format_args(args...));
	std::abort();
}

} // Nibbler

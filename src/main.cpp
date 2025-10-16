#include "Nibbler/Nibbler.hpp"

struct Arguments
{
	Nibbler::RendererAPI::API api = Nibbler::RendererAPI::API::SDL3;
	int8_t	game_width = 20;
	int8_t	game_height = 20;
	bool	parent = true;
	bool	no_client = false;
	std::chrono::seconds timeout = std::chrono::seconds(10);
	const char* host = "localhost";
	int			port = 8080;
	bool		pvp = false;
	std::chrono::milliseconds	death = std::chrono::milliseconds(1000);
	std::chrono::milliseconds	respawn = std::chrono::milliseconds(1000);
};

static const char *help_message = R"(Usage: nibbler [options]
  Options:
    --help        Show the usage message and exit.
    --backend  -b Specify the rendering backend.
    --width    -w Set the width of the game area. Use with --height.
    --height   -h Set the height of the game area. Use with --width.
    --server,  -s Only run the server, without a game window.
    --client,  -c Only run the client, without a server. A game server should be running (see --host, --port).
    --timeout     Set the timeout.
    --host        Set the host server address.
    --port     -p Set the host server port.
	--pvp         Enable PvP. Snakes can kill each other.
	--death    -d Set the death timer duration in ms (default=1000ms).
	--respawn  -r Set the respawn invincibility timer duration in ms (default=1000ms).

Default game size is 20,20.
Available backends are 'SDL3', 'GLFW' or 'Allegro'.
Default rendering backend is SDL3.)";

static void	show_help_message_and_exit()
{
	std::cout << help_message << std::endl;
	std::exit(0);
}

static void	throw_unknown_option(const char* program_name, const char *option)
{
	std::cerr << "Nibbler: unknown option '" << option << "'" << std::endl
	<< "Try './" << program_name << " --help' for more information." << std::endl;
	std::exit(1);
}

static void	throw_missing_argument(const char *option)
{
	std::cerr << "Nibbler: option '" << option << "' requires an argument" << std::endl;
	std::exit(1);
}

static void	throw_invalid_argument(const char *option, const char *argument, const char *usage)
{
	std::cerr << "Nibbler: invalid argument '" << argument << "' for option '" << option << "'" << std::endl;
	std::cerr << usage << std::endl;
	std::exit(1);
}

static bool	parse_options(const int argc, const char *const *argv, Arguments& arguments)
{
	bool size;

	if (argc == 1)
		return true;

	for (int i = 1; i < argc; i++)
	{
		if (std::strcmp(argv[i], "--help") == 0)
			show_help_message_and_exit();

		if (std::strcmp(argv[i], "-w") == 0 || std::strcmp(argv[i], "-h") == 0
			|| std::strcmp(argv[i], "--width") == 0|| std::strcmp(argv[i], "--height") == 0)
		{
			if (!argv[i + 1])
				throw_missing_argument(argv[i]);
			i++;
			for (int j = 0; argv[i][j]; j++)
				if (!std::isdigit(argv[i][j]))
					throw_invalid_argument(argv[i - 1], argv[i], "Expected a numeric positive value");
			if (argv[i - 1][1] == 'w' || argv[i - 1][2] == 'w')
			{
				arguments.game_width = std::atoi(argv[i]);
				if (arguments.game_width < 7)
					throw_invalid_argument(argv[i - 1], argv[i], "Minimum area size is 7");
				if (arguments.game_width > 50)
					throw_invalid_argument(argv[i - 1], argv[i], "Nibbler: maximum area size is 50");
			}
			else
			{
				arguments.game_height = std::atoi(argv[i]);
				if (arguments.game_height < 7)
					throw_invalid_argument(argv[i - 1], argv[i], "Minimum area size is 7");
				if (arguments.game_height > 50)
					throw_invalid_argument(argv[i - 1], argv[i], "Nibbler: maximum area size is 50");
			}
			size = true;
		}
		else if (std::strcmp(argv[i], "-b") == 0 || std::strcmp(argv[i], "--backend") == 0)
		{
			if (!argv[i + 1])
				throw_missing_argument(argv[i]);
			i++;
			if (std::strcmp(argv[i], "SDL3") == 0 || std::strcmp(argv[i], "SDL") == 0
				|| std::strcmp(argv[i], "sdl") == 0 || std::strcmp(argv[i], "sdl3") == 0)
				arguments.api = Nibbler::RendererAPI::API::SDL3;
			else if (std::strcmp(argv[i], "GLFW") == 0 || std::strcmp(argv[i], "glfw") == 0)
				arguments.api = Nibbler::RendererAPI::API::GLFW;
			else if (std::strcmp(argv[i], "Allegro") == 0 || std::strcmp(argv[i], "allegro") == 0)
				arguments.api = Nibbler::RendererAPI::API::Allegro;
			else
				throw_invalid_argument(argv[i - 1], argv[i], "Available backends are:\n  - 'SDL3', 'GLFW', or 'Allegro'");
			size = true;
		}
		else if (std::strcmp(argv[i], "-s") == 0 || std::strcmp(argv[i], "--server") == 0)
		{
			arguments.parent = true;
			arguments.no_client = true;
		}
		else if (std::strcmp(argv[i], "-j") == 0 || std::strcmp(argv[i], "--join") == 0 || std::strcmp(argv[i], "--client") == 0)
		{
			arguments.parent = false;
			arguments.no_client = false;
		}
		else if (std::strcmp(argv[i], "--timeout") == 0)
		{
			if (!argv[i + 1])
				throw_missing_argument(argv[i]);
			i++;
			for (int j = 0; argv[i][j]; j++)
				if (!std::isdigit(argv[i][j]) && argv[i][j] != '-')
					throw_invalid_argument(argv[i - 1], argv[i], "Expected a numeric positive value or -1");
			arguments.timeout = std::chrono::seconds(std::atoi(argv[i]));
		}
		else if (std::strcmp(argv[i], "--host") == 0)
		{
			if (!argv[i + 1])
				throw_missing_argument(argv[i]);
			i++;
			for (int j = 0; argv[i][j]; j++)
				if (!std::isalnum(argv[i][j]) && argv[i][j] != '.')
					throw_invalid_argument(argv[i - 1], argv[i], "Invalid host");
			arguments.host = argv[i];
		}
		else if (std::strcmp(argv[i], "--port") == 0 || std::strcmp(argv[i], "-p") == 0)
		{
			if (!argv[i + 1])
				throw_missing_argument(argv[i]);
			i++;
			for (int j = 0; argv[i][j]; j++)
				if (!std::isdigit(argv[i][j]))
					throw_invalid_argument(argv[i - 1], argv[i], "Invalid port");
			arguments.port = std::atoi(argv[i]);
		}
		else if (std::strcmp(argv[i], "--pvp") == 0)
		{
			arguments.pvp = true;
		}
		else if (std::strcmp(argv[i], "--death") == 0 || std::strcmp(argv[i], "-d") == 0)
		{
			if (!argv[i + 1])
				throw_missing_argument(argv[i]);
			i++;
			for (int j = 0; argv[i][j]; j++)
				if (!std::isdigit(argv[i][j]) && argv[i][j] != '-')
					throw_invalid_argument(argv[i - 1], argv[i], "Expected a numeric positive value");
			arguments.death = std::chrono::milliseconds(std::atoi(argv[i]));
		}
		else if (std::strcmp(argv[i], "--respawn") == 0 || std::strcmp(argv[i], "-r") == 0)
		{
			if (!argv[i + 1])
				throw_missing_argument(argv[i]);
			i++;
			for (int j = 0; argv[i][j]; j++)
				if (!std::isdigit(argv[i][j]) && argv[i][j] != '-')
					throw_invalid_argument(argv[i - 1], argv[i], "Expected a numeric positive value");
			arguments.respawn = std::chrono::milliseconds(std::atoi(argv[i]));
		}
		else
			throw_unknown_option(argv[0], argv[i]);
	}
	
	if (!arguments.parent && size)
		Nibbler::Log::Warn("Size argument ignored: not a server");
	return true;
}

static void	run_client(Arguments arguments)
{
	Nibbler::Timer timer;

	if (arguments.no_client)
		return ;

	Nibbler::ClientConfig config = {
		.api = arguments.api,
		.host = arguments.host,
		.port = arguments.port,
		.timeout = arguments.timeout,
	};

	Nibbler::Client	client(config);
	client.Run();
}

static void	run_server(Arguments arguments)
{
	int _area_size_difference = std::abs(arguments.game_width - arguments.game_height);
	_area_size_difference = _area_size_difference ? _area_size_difference : 1;
	int tick_rate = ((arguments.game_width + arguments.game_height) / 2) * _area_size_difference;
	uint64_t ns_per_tick = std::max(Nibbler::NS_PER_SECOND / tick_rate, Nibbler::NS_PER_TICK);

	Nibbler::ServerConfig config = {
		.host = arguments.host,
		.port = arguments.port,
		.game_width = arguments.game_width,
		.game_height = arguments.game_height,
		.ns_per_tick = ns_per_tick,
		.pvp = arguments.pvp,
		.death = arguments.death,
		.respawn = arguments.respawn,
	};

	Nibbler::Server::Init(config);
	Nibbler::Server::Run();
}

int	main(int argc, char **argv)
{
	Nibbler::Timer timer;
	Arguments arguments;

	Nibbler::Log::Init();

	if (!parse_options(argc, argv, arguments))
		return (1);
	
#ifdef _WIN32
	int status;
	WSADATA	wsaData;
	status = WSAStartup(WINSOCK_VERSION, &wsaData);
	NIB_ASSERT(status == 0, "WSAStartup(): {}", WSAGetLastError());

	NIB_NOTUSED(status);
#endif

	Nibbler::g_ApplicationRunning = true;

	if (arguments.parent)
	{
		if (arguments.no_client)
		{
			run_server(arguments);
		}
		else
		{
			std::thread t1(run_server, arguments);
			run_client(arguments);
			t1.join();
			Nibbler::Server::Shutdown();
		}
	}
	else
	{
		run_client(arguments);
	}

#ifdef _WIN32
	WSACleanup();
#endif
	return (0);
}

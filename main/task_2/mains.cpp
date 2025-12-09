module main.task_2;

import <map>;
import <string_view>;
import <iostream>;
import <print>;

import square;
import parallel.process;

namespace mains
{
	namespace task2
	{
		template< class Manager >
		int common_commander(int argc, const char*const* argv);
	}
}

int mains::task2::pipe(int argc, const char*const* argv)
{
	using namespace parallel::process;
	return common_commander< windows_manager< control_policy::anonymous_tube,
					notify_policy::await_thread > >(argc, argv);
}
int mains::task2::bin_pipe(int argc, const char*const* argv)
{
	using namespace parallel::process;
	return common_commander< windows_manager< control_policy::bin_anonymous_tube,
					notify_policy::await_thread > >(argc, argv);
}
template< class Manager >
int mains::task2::common_commander(int argc, const char*const* argv)
{
	using parser_t = user_context< Manager >;
	const std::map< std::string, void(*)(parser_t&) > commands_set = {
				{"spawn", &create_process},
				{"alive", &print_alive},
				{"kill", &kill},
				{"frame", &get_frame},
				{"frameset", &get_set_frame},
				{"circle", &create_circle},
				{"rectangle", &create_rectangle},
				{"set", &create_set},
				{"show", &show_figure},
				{"showall", &show_shapes},
				{"showset", &show_set},
				{"showallset", &show_sets},
				{"areaon", &create_task}
			};
	parser_t parser{std::cin, std::cout, std::cerr, argv[0], argv[1]};
	std::string command;
	while (std::cin >> command)
	{
		auto pos = commands_set.find(command);
		if (pos == commands_set.end())
		{
			std::println(std::cerr, "unknown command");
			continue;
		}
		try
		{
			std::cin.exceptions(std::ios_base::failbit);
			(pos->second)(parser);
		}
		catch (const std::ios_base::failure&)
		{
			std::println(std::cerr, "failed to parse command (skipping line)");
			std::cin.exceptions(std::ios_base::goodbit);
			std::cin.clear(std::cin.rdstate() & ~std::ios_base::failbit);
			std::cin.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
		}
		catch (const std::exception& e)
		{
			std::println(std::cerr, "failed to execute command: {}", e.what());
		}
		std::cin.exceptions(std::ios_base::goodbit);
	}
	return 0;
}

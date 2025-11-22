module main.task_2;

import <string_view>;
import <string>;
import <map>;
import <iostream>;
import <print>;

import square;
import parallel.thread;
import parallel.process;
import main.dispatch;

namespace mains
{
	namespace task2
	{
		template< class Manager >
		int common_commander(int argc, char** argv);
		template< class Manager >
		struct user_commands_parser
		{
			std::ostream& out;
			std::ostream& err;
			const char* program;

			std::map< std::string, square::composition::shape > shapes;
			std::map< std::string, square::composition > compositions;
			Manager man;

			void add_process(std::istream& in);
			void print_alive(std::istream& in);
			void kill(std::istream& in);
		};
		
	}
}

int mains::task2::pipe(int argc, char** argv)
{
	using namespace parallel::process;
	return common_commander< windows_manager< control_policy::anonymous_tube,
					notify_policy::await_thread > >(argc, argv);
}
int mains::task2::stdin_executor(int argc, char** argv)
{
	if (argc < 2)
	{
		std::println(std::cerr, "executor was started without seed");
		return 1;
	}
	std::uint64_t seed = 0;
	try
	{
		seed = std::stoull(argv[1]);
	}
	catch (const std::exception& e)
	{
		std::println(std::cerr, "executor failed to parse seed: {}", e.what());
		return 1;
	}

	square::composition pred;
	{
		pred.shapes.push_back(square::circle_t{1.0f, {0.0f, 0.0f}});
		square::settings set = {{{-1, -1}, {1, 1}}, 9, 100000000ULL, seed};
		try
		{
			std::uint64_t entries =
					square::count_entries(parallel::thread::std_manager< std::uint64_t >(set.nthreads - 1), set, pred);
			std::println("{:.4f}", square::square_tr(entries, set.whole_cycles, set.frame));
		}
		catch (const std::system_error& err)
		{
			std::println(std::cerr, "os error: {}", err.what());
			return 2;
		}
		pred.shapes.resize(0);
	}

	char c;
	while (std::cin >> c)
	{}
	return 0;
}
template< class Manager >
int mains::task2::common_commander(int argc, char** argv)
{
	using parser_t = user_commands_parser< Manager >;
	const std::map< std::string, void(parser_t::*)(std::istream&) > commands_set = {
				{"spawn", &parser_t::add_process},
				{"alive", &parser_t::print_alive},
				{"kill", &parser_t::kill}
			};
	parser_t parser{std::cout, std::cerr, argv[0]};
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
			(parser.*(pos->second))(std::cin);
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
template< class Manager >
void mains::task2::user_commands_parser< Manager >::add_process(std::istream& in)
{
	std::string name;
	std::uint64_t seed = 0;
	in >> name >> seed;
	if (!man.create_process(std::move(name), program,
				std::string{program} + ' ' + mains::dispatcher::executor_mark + ' ' + std::to_string(seed)))
	{
		std::println(err, "process with the same name already exists");
	}
}
template< class Manager >
void mains::task2::user_commands_parser< Manager >::print_alive(std::istream& in)
{
	size_t lives = 0;
	for (const std::string& name : man.process_names())
	{
		if (man.alive(name))
		{
			lives++;
			std::println("{}", name);
		}
	}
	if (lives == 0)
	{
		std::println("noone alive");
	}
}
template< class Manager >
void mains::task2::user_commands_parser< Manager >::kill(std::istream& in)
{
	std::string name;
	in >> name;
	man.kill(name);
}

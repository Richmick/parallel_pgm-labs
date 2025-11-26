module main.task_1;

//import std;
import <cstdint>;
import <print>;
import <iostream>;
import <system_error>;
import <execution>;
import <algorithm>;
import <random>;
import <ranges>;
import <future>;

import square;
import parallel.thread;

namespace mains
{
	namespace task1
	{
		template< class Manager >
		int common(int argc, const char*const* argv);
		struct random;
		struct entry_generator;
	}
}

template< class Manager >
int mains::task1::common(int argc, const char*const* argv)
{
	square::settings set;
	square::in_shape entry_pred{square::circle_t{0.0f, {0.0f, 0.0f}}};
	try
	{
		parse_argv(argc, argv, set);
		parse_istream(std::cin, set, entry_pred.shape.radius);
	}
	catch (const std::exception& err)
	{
		std::println(std::cerr, "failed to parse input: {}", err.what());
		return 1;
	}

	try
	{
		std::uint64_t entries = count_entries(Manager(set.nthreads - 1), set, entry_pred);
		std::println("{:.4f}", square::square_tr(entries, set.whole_cycles, set.frame));
	}
	catch (const std::system_error& err)
	{
		std::println(std::cerr, "os error: {}", err.what());
		return 2;
	}
	return 0;
}
int mains::task1::std_thread(int argc, const char*const* argv)
{
	return common< parallel::thread::std_manager< std::uint64_t > >(argc, argv);
}
int mains::task1::future_thread(int argc, const char*const* argv)
{
	return common< parallel::thread::future_manager< std::uint64_t > >(argc, argv);
}
int mains::task1::future_async(int argc, const char*const* argv)
{
	return common< parallel::thread::future_async_manager< std::uint64_t > >(argc, argv);
}
int mains::task1::os_dependent(int argc, const char*const* argv)
{
	return common< parallel::thread::os_manager< std::uint64_t > >(argc, argv);
}

struct mains::task1::random
{
	std::mt19937_64 engine;
	random(std::uint64_t seed):
		engine(seed)
	{}
	random(const random& src):
		engine(src.engine)
	{
		engine.seed(engine());
	}
};
struct mains::task1::entry_generator
{
	random random_engine;
	square::in_shape< square::circle_t > predicate;
	std::uniform_real_distribution< float > x_distribution;
	std::uniform_real_distribution< float > y_distribution;

	std::size_t operator()(std::size_t)
	{
		return predicate({x_distribution(random_engine.engine), y_distribution(random_engine.engine)});
	}
};
int mains::task1::algorithm(int argc, const char*const* argv)
{
#ifdef __cpp_lib_execution
	square::settings set;
	square::in_shape entry_pred{square::circle_t{0.0f, {0.0f, 0.0f}}};
	try
	{
		parse_argv(argc, argv, set);
		parse_istream(std::cin, set, entry_pred.shape.radius);
	}
	catch (const std::exception& err)
	{
		std::println(std::cerr, "failed to parse input: {}", err.what());
		return 1;
	}
	auto view = std::ranges::iota_view{std::uint64_t{0}, set.whole_cycles};
	std::uniform_real_distribution coord_distribution{0.0f, entry_pred.shape.radius};
	entry_generator gen{{set.seed}, entry_pred, coord_distribution, coord_distribution};
	std::uint64_t entries = std::transform_reduce(std::execution::par, view.begin(), view.end(),
					std::uint64_t{0}, std::plus<>{}, gen);
	std::println("{:.4f}", square::square_tr(entries, set.whole_cycles, set.frame));
	return 0;
#else
	std::println(std::cerr, "execution::par is unsupported by used compiller");
	return 3;
#endif
}

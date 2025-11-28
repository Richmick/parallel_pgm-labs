export module square;

//import std;
import <cstddef>;
import <cstdint>;
import <random>;
import <ranges>;
import <concepts>;

export import :shapes;
export import :predicates;
export import :format;

namespace square
{
	export struct settings
	{
		rect_t frame;
		std::size_t nthreads = 1;
		std::uint64_t whole_cycles = 1;
		std::uint64_t seed = 0;
	};

	export template< std::predicate< square::point_t > P >
	std::uint64_t count_entries(settings set, P pred);
	export template< class M, std::predicate< square::point_t > P >
	std::uint64_t count_entries(M thread_manager, settings set, P pred);
}

template< std::predicate< square::point_t > P >
std::uint64_t square::count_entries(settings set, P pred)
{
	std::mt19937_64 random_engine(set.seed);
	std::uniform_real_distribution x_distribution(set.frame.p1.x, set.frame.p2.x);
	std::uniform_real_distribution y_distribution(set.frame.p1.y, set.frame.p2.y);

	std::uint64_t result = 0;
	for (std::uint64_t i = 0; i < set.whole_cycles; i++)
	{
		result += (bool)(pred(point_t{x_distribution(random_engine), y_distribution(random_engine)}));
	}
	return result;
}
template< class M, std::predicate< square::point_t > P >
std::uint64_t square::count_entries(M thread_manager, settings set, P pred)
{
	std::mt19937_64 random_engine(set.seed);
	auto nthreads_range = std::ranges::iota_view{0UZ, set.nthreads - 1};

	std::uint64_t single_thread_cycles = set.whole_cycles / set.nthreads;
	std::uint64_t this_thread_cycles = single_thread_cycles + set.whole_cycles % set.nthreads;
	set.whole_cycles = single_thread_cycles;
	for (std::size_t i: nthreads_range)
	{
		set.seed = random_engine();
		thread_manager.push_thread(count_entries< P >, set, pred);
	}

	set.seed = random_engine();
	set.whole_cycles = this_thread_cycles;
	std::uint64_t counted = count_entries(set, pred);
	for (std::size_t i: nthreads_range)
	{
		counted += thread_manager.await_value(i);
	}
	return counted;
}

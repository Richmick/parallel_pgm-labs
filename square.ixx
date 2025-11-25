export module square;

//import std;
import <cstddef>;
import <cstdint>;
import <random>;
import <ranges>;
import <concepts>;
import <variant>;
import <vector>;
import <algorithm>;
import <format>;

namespace square
{
	export struct point_t
	{
		float x, y;
	};
	export constexpr float distance_sqr(point_t p1, point_t p2);
	export struct rect_t
	{
		point_t p1, p2;
	};
	export struct circle_t
	{
		float radius;
		point_t center;
	};
	export constexpr float square_of(rect_t rect);
	export constexpr rect_t merge_frames(rect_t lhs, rect_t rhs);

	export constexpr float square_tr(std::uint64_t entries, std::uint64_t nchecks, rect_t frame);

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

	export struct get_frame
	{
		static constexpr rect_t operator()(const rect_t& shape)
		{
			return shape;
		}
		static constexpr rect_t operator()(const circle_t& shape)
		{
			return {{shape.center.x - shape.radius, shape.center.y - shape.radius},
						{shape.center.x + shape.radius, shape.center.y + shape.radius}};
		}
		template< class... Shapes >
		static constexpr rect_t operator()(const std::variant< Shapes... >& shape)
		{
			return std::visit(get_frame{}, shape);
		}
	};
	export struct is_inside
	{
		point_t point;
		constexpr bool operator()(const rect_t& shape)
		{
			return (shape.p1.x >= point.x) && (shape.p2.x <= point.x)
					&& (shape.p1.y >= point.y) && (shape.p2.y <= point.y);
		}
		constexpr bool operator()(const circle_t& shape)
		{
			return distance_sqr(point, shape.center) <= shape.radius * shape.radius;
		}
		template< class... Shapes >
		constexpr bool operator()(const std::variant< Shapes... >& shape)
		{
			return std::visit(*this, shape);
		}
	};
	export template< class Shape >
	struct in_shape
	{
		Shape shape;
		constexpr bool operator()(point_t p) const
		{
			return is_inside{p}(shape);
		}
	};
	export struct composition
	{
		using shape = std::variant< rect_t, circle_t >;
		std::vector< shape > shapes;
		constexpr bool operator()(point_t p)
		{
			is_inside pred{p};
			for (const shape& i : shapes)
			{
				if (pred(i))
				{
					return true;
				}
			}
			return false;
		}
	};
}
export template<>
struct std::formatter< square::point_t >: std::formatter< std::string >
{
	auto format(square::point_t p, std::format_context& ctx) const
	{
		return std::formatter< std::string >::format(std::format("{{{}, {}}}", p.x, p.y), ctx);
	}
};
export template<>
struct std::formatter< square::rect_t >: std::formatter< std::string >
{
	auto format(square::rect_t r, std::format_context& ctx) const
	{
		return std::formatter< std::string >::format(std::format("{{{}, {}}}", r.p1, r.p2), ctx);
	}
};

constexpr float square::distance_sqr(point_t p1, point_t p2)
{
	p1.x -= p2.x;
	p1.y -= p2.y;
	return p1.x * p1.x + p1.y * p1.y;
}
constexpr float square::square_of(rect_t rect)
{
	rect.p1.x -= rect.p2.x;
	rect.p1.y -= rect.p2.y;
	return rect.p1.x * rect.p1.y;
}
constexpr square::rect_t square::merge_frames(rect_t lhs, rect_t rhs)
{
	return {{std::min(lhs.p1.x, rhs.p1.x), std::min(lhs.p1.y, rhs.p1.y)},
				{std::max(lhs.p2.x, rhs.p2.x), std::max(lhs.p2.y, rhs.p2.y)}};
}
constexpr float square::square_tr(std::uint64_t entries, std::uint64_t nchecks, rect_t frame)
{
	return 4 * square_of(frame) * entries / nchecks;
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

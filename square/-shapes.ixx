export module square:shapes;

import <cstddef>;
import <cstdint>;
import <algorithm>;
import <variant>;
import <vector>;

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
	
	export struct composition
	{
		using shape = std::variant< rect_t, circle_t >;
		std::vector< shape > shapes;
	};
}

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

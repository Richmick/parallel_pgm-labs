export module square:predicates;

import <algorithm>;
import :shapes;

namespace square
{
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
		static constexpr rect_t operator()(const composition& shape)
		{
			if (shape.shapes.size() == 0)
			{
				return {};
			}
			square::rect_t res = operator()(shape.shapes.front());
			for (std::size_t i = 1; i < shape.shapes.size(); i++)
			{
				res = square::merge_frames(res, operator()(shape.shapes[i]));
			}
			return res;
		}
	};
	export struct is_inside
	{
		point_t point;
		constexpr bool operator()(const rect_t& shape)
		{
			return (shape.p1.x <= point.x) && (shape.p2.x >= point.x)
				&& (shape.p1.y <= point.y) && (shape.p2.y >= point.y);
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
		constexpr bool operator()(const composition& shape)
		{
			return std::ranges::any_of(shape.shapes, *this);
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
}

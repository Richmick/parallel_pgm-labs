export module square:format;

import <format>;
import <algorithm>;
import <tuple>;
import format_help;
import :shapes;

namespace helpers
{
	// [{<n>}][s][|]
	// <n> - name argument id
	// s, | - separators
	class shape_name_formatter
	{
	public:
		constexpr std::format_parse_context::iterator parse(std::format_parse_context& ctx)
		{
			auto it = ctx.begin();
			if ((it != ctx.end()) && (*it == '{'))
			{
				it = name_id_.parse(ctx);
			}
			if ((it != ctx.end()) && (*it == 's'))
			{
				++it;
			}
			if ((it != ctx.end()) && (*it == '|'))
			{
				++it;
			}
			return it;
		}
		std::format_context::iterator format(std::format_context& ctx) const
		{
			if (has_name())
			{
				return std::ranges::copy(std::visit_format_arg(extractor{}, ctx.arg(name_id_.raw())),
							ctx.out()).out;
			}
			return ctx.out();
		}
		constexpr bool has_name() const noexcept
		{
			return !name_id_.empty();
		}

	private:
		number_format_helper name_id_;

		struct extractor
		{
			std::string_view operator()(const char* str) {return str;}
			std::string_view operator()(std::string_view str) {return str;}
			std::string_view operator()(auto i)
			{
				throw std::format_error("required integral value");
			}
		};
	};
	// [!][{<name-id>}][s][|][{<nth-format>}]...
	// ! - hide shape type
	template< class... Types >
	class common_shape_formatter:
		protected ::helpers::shape_name_formatter,
		protected conveyer_formatter< Types... >
	{
	public:
		constexpr std::format_parse_context::iterator parse(std::format_parse_context& ctx)
		{
			hide_shape_type = false;
			auto it = ctx.begin();
			if ((it != ctx.end()) && (*it == '!'))
			{
				hide_shape_type = true;
				ctx.advance_to(++it);
			}
			ctx.advance_to(helpers::shape_name_formatter::parse(ctx));
			return conveyer_formatter< Types... >::parse(ctx);
		}
		std::format_context::iterator format_with_name(const char* figure, std::format_context& ctx) const
		{
			std::format_context::iterator it = ctx.out();
			if (!hide_shape_type)
			{
				it = std::format_to(it, "{} ", figure);
			}
			if (has_name())
			{
				ctx.advance_to(it);
				it = helpers::shape_name_formatter::format(ctx);
				*(it++) = ' ';
			}
			return it;
		}
	private:
		bool hide_shape_type;
	};
}

// [#][<0 - 3 symbols>][|][float-specs]
// # - use braces like {0.0 0.0}
// <0 - 3 symbols> - coordinates separator
export template<>
struct std::formatter< square::point_t >: private std::formatter< float >
{
	static constexpr inline std::size_t max_separator_length = 3;

	constexpr std::format_parse_context::iterator parse(std::format_parse_context& ctx)
	{
		auto it = ctx.begin();
		if ((it != ctx.end()) && (*it == '#'))
		{
			it++;
			braced_ = true;
		}
		std::size_t i = 0;
		while ((it != ctx.end()) && (*it != '|') && (*it != '}'))
		{
			if (i >= max_separator_length)
			{
				throw std::format_error("separator is too long");
			}
			separator_[i++] = *(it++);
		}
		if (i != 0)
		{
			separator_[i] = '\0';
		}
		if ((it != ctx.end()) && (*it == '|'))
		{
			++it;
		}
		ctx.advance_to(it);
		return std::formatter< float >::parse(ctx);
	}
	std::format_context::iterator format(square::point_t p, std::format_context& ctx) const
	{
		const std::formatter< float >& base = *this;
		auto it = ctx.out();
		if (braced_)
		{
			*it = '{';
			ctx.advance_to(++it);
		}
		it = base.format(p.x, ctx);
		for (std::size_t i = 0; separator_[i] != 0; *(it++) = separator_[i], i++)
		{}
		ctx.advance_to(it);
		it = base.format(p.y, ctx);
		if (braced_)
		{
			*(it++) = '}';
		}
		return it;
	}
private:
	char separator_[max_separator_length + 1]{' '};
	bool braced_ = false;
};
// [!][{<name-id>}][s][|][{<points-format>}][f]
// <points-format> = [#][<0 - 3 symbols>][|][float-specs]
export template<>
struct std::formatter< square::rect_t >:
	public helpers::common_shape_formatter< square::point_t >
{
	std::format_context::iterator format(const square::rect_t& r, std::format_context& ctx) const
	{
		auto it = format_with_name("rectangle", ctx);
		it = std::get< 0 >(conveyer).format(r.p1, ctx);
		*(it++) = ' '; ctx.advance_to(it);
		return std::get< 0 >(conveyer).format(r.p2, ctx);
	}
};
// [!][{<name-id>}][s][|][{<radius-format>}][{<center-format>}][f]
// <radius-format> = [float-specs]
// <point-format> = [#][<0 - 3 symbols>][|][float-specs]
export template<>
struct std::formatter< square::circle_t >:
	public helpers::common_shape_formatter< float, square::point_t >
{
	std::format_context::iterator format(const square::circle_t& c, std::format_context& ctx) const
	{
		auto it = format_with_name("circle", ctx);
		it = std::get< 0 >(conveyer).format(c.radius, ctx);
		*(it++) = ' '; ctx.advance_to(it);
		return std::get< 1 >(conveyer).format(c.center, ctx);
	}
};

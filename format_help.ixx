export module format_help;

import <format>;
import <limits>;
import <tuple>;
import <utility>;

namespace helpers
{
	export class number_format_helper
	{
	public:
		constexpr std::format_parse_context::iterator manual_parse(std::format_parse_context& ctx)
		{
			auto it = ctx.begin();
			value_ = 0;
			for (; (it != ctx.end()) && (*it >= '0') && (*it <= '9'); ++it)
			{
				char c = *it - '0';
				if (value_ > (std::numeric_limits< std::size_t >::max() - c) / 10)
				{
					throw std::format_error("number overflow");
				}
				value_ = value_ * 10 + c;
			}
			return it;
		}
		constexpr std::format_parse_context::iterator parse(std::format_parse_context& ctx)
		{
			auto it = ctx.begin();
			if ((it == ctx.end()) || (*it == '}'))
			{
				throw std::format_error("no number found");
			}
			if ((*it >= '0') && (*it <= '9'))
			{
				it = manual_parse(ctx);
				category_ = value_category::value;
			}
			else if (*it == '{')
			{
				if (++it == ctx.end())
				{
					throw std::format_error("indexing not finished");
				}
				ctx.advance_to(it);
				if (*it == '}')
				{
					value_ = ctx.next_arg_id();
					it = ctx.begin();
					category_ = value_category::link;
				}
				else
				{
					it = manual_parse(ctx);
					if ((it == ctx.end()) || (*it != '}'))
					{
						throw std::format_error("indexing not finished");
					}
					ctx.check_arg_id(value_);
					category_ = value_category::link;
				}
				++it;
			}
			else
			{
				category_ = value_category::empty;
				throw std::format_error("no digit char in number");
			}
			return it;
		}
		constexpr bool empty() const noexcept
		{
			return category_ == value_category::empty;
		}
		constexpr bool is_link() const noexcept
		{
			return category_ == value_category::link;
		}
		constexpr bool is_hard_value() const noexcept
		{
			return category_ == value_category::value;
		}
		std::size_t get(std::format_context& ctx)
		{
			switch (category_)
			{
			case value_category::link:
				return std::visit_format_arg(extractor{}, ctx.arg(value_));
			case value_category::value:
				return value_;
			default:
				throw std::format_error("value were not set");
			}
		}
		constexpr std::size_t raw() const noexcept
		{
			return value_;
		}
	private:
		std::size_t value_ = ~0UZ;
		enum class value_category
		{
			empty, value, link
		};
		value_category category_ = value_category::empty;

		struct extractor
		{
			std::size_t operator()(auto i)
			{
				if constexpr (std::is_integral_v< decltype(i) >)
				{
					return i;
				}
				else
				{
					throw std::format_error("required integral value");
				}
			}
		};
	};
	export template< class... Types >
	class conveyer_formatter
	{
	public:
		std::tuple< std::formatter< Types >... > conveyer;
		constexpr std::format_parse_context::iterator parse(std::format_parse_context& ctx)
		{
			parse_conveyer(ctx, std::index_sequence_for< Types... >{});
			return ctx.begin();
		}

	private:
		template< std::size_t... Idxs >
		constexpr void parse_conveyer(std::format_parse_context& ctx, std::index_sequence< Idxs... >)
		{
			apply_conveyer< Idxs... >(ctx);
		}
		template< std::size_t I, std::size_t... Idxs >
		constexpr void apply_conveyer(std::format_parse_context& ctx)
		{
			auto it = ctx.begin();
			if ((it == ctx.end()) || (*it == '}'))
			{
				return;
			}
			if (*it != '{')
			{
				throw std::format_error("formatters conveyer requires formates in {}");
			}
			ctx.advance_to(std::get< I >(conveyer).parse(ctx));
			if ((it == ctx.end()) || (*it != '}'))
			{
				throw std::format_error("not found '}'");
			}
			if constexpr (sizeof...(Idxs) != 0)
			{
				apply_conveyer< Idxs... >(ctx);
			}
		}
	};
}

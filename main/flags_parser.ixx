export module main.flags_parser;

import <span>;
import <map>;
import <set>;
import <string_view>;

namespace mains
{
	export class flags_parser
	{
	public:
		std::set< std::string_view > flags;
		std::map< std::string_view, std::string_view > variables;

		flags_parser() = default;
		flags_parser(std::span< const char*const > args)
		{
			parse(args);
		}

		void parse(std::span< const char*const > args)
		{
			for (std::string_view arg: args)
			{
				if (!arg.starts_with("--"))
				{
					continue;
				}
				arg.remove_prefix(2);

				std::size_t equals_sign = arg.find('=');
				if (equals_sign != std::string_view::npos)
				{
					auto placeholder = variables.try_emplace(arg.substr(0, equals_sign), arg.substr(equals_sign + 1)).second;
				}
				else
				{
					flags.emplace(arg);
				}
			}
		}
		std::string_view operator[](std::string_view key)
		{
			auto it = variables.find(key);
			if (it != variables.end())
			{
				return it->second;
			}
			return {};
		}
		void clear()
		{
			flags.clear();
			variables.clear();
		}
	};
}

export module main.dispatch;

import <vector>;
import <span>;
import <algorithm>;
import <map>;
import <set>;
import <string_view>;

import main.flags_parser;

namespace mains
{
	export struct dispatch_option
	{
		using executable = int(&)(int, const char*const*);

		executable task;
		const char* group = "";
		const char* tag = "";
		bool erase_dispatch_key = false;
		std::span< const char*const > required_flags;
		std::span< const char*const > required_vars;
	};
	export int call_main(std::span< const char*const > args, std::span< const dispatch_option > options)
	{
		flags_parser flags(args.subspan(1));
		for (const dispatch_option& opt: options)
		{
			if (flags[opt.group] != opt.tag)
			{
				continue;
			}

			bool flags_match = std::ranges::all_of(opt.required_flags,
						[&flags](auto s){ return flags.flags.contains(s); });
			bool vars_match = std::ranges::all_of(opt.required_vars,
						[&flags](auto s){ return flags.variables.contains(s); });
			if (!flags_match || !vars_match)
			{
				continue;
			}

			flags.clear();
			if (opt.erase_dispatch_key)
			{
				std::vector< const char* > parsed_args;
				parsed_args.reserve(args.size() - 1);
				parsed_args.push_back(args.front());
				parsed_args.append_range(args.subspan(2));
				return opt.task(static_cast< int >(parsed_args.size()), parsed_args.data());
			}
			return opt.task(static_cast< int >(args.size()), args.data());
		}
		return 3;
	}
}

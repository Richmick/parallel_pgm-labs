export module main.dispatch;

import <map>;
import <vector>;
import <span>;
import <string>;
import <string_view>;

import main.task_1;
import main.task_2;

namespace mains
{
	struct dispatch_option
	{
		using executable = int(&)(int, char**);

		executable task;
		bool executor = false;
		const char* group = "";
		const char* tag = "";
		std::span< const char*const > required_flags;
	};
	export struct dispatcher
	{
		static constexpr inline char executor_mark[] = "--executor";
		static constexpr inline char thread_main_mark[] = "--thread-main";
		static constexpr inline char process_main_mark[] = "--process-main";

		static constexpr const char* shared_mem_flags[] = {"--mem"};

		static constexpr dispatch_option options[] = {
					{task1::std_thread, false, thread_main_mark, "std"},
					{task1::future_thread, false, thread_main_mark, "std+future"},
					{task1::future_async, false, thread_main_mark, "std+async"},
					{task1::os_dependent, false, thread_main_mark, "os"},
					{task1::algorithm, false, thread_main_mark, "algorithm"},

					{task2::pipe, false, process_main_mark, "pipe"},
					{task2::stdin_executor, true, process_main_mark, "pipe"}
					//{task2::shared_mem, false, process_main_mark, "shared_mem", shared_mem_flags},
				};

		bool executor = false;
		std::map< std::string_view, std::string_view > variables;
		std::vector< char* > args;

		dispatcher() = default;
		dispatcher(int argc, char** argv):
			args(argv, argv + argc)
		{
			for (std::string_view argument: args)
			{
				if (argument == executor_mark)
				{
					executor = true;
				}
				std::size_t equals_sign = argument.find('=');
				if (equals_sign != std::string_view::npos)
				{
					variables.try_emplace(argument.substr(0, equals_sign), argument.substr(equals_sign + 1)).second;
				}
			}
		}

		int operator()()
		{
			for (const dispatch_option& opt: options)
			{
				std::size_t ndispatch_keys = 1;
				if (executor != opt.executor)
				{ continue; }

				if (!executor)
				{
					auto tag = variables.find(opt.group);
					if ((tag == variables.end()) || (tag->second != opt.tag))
					{
						continue;
					}
				}

				for (const char* key: opt.required_flags)
				{
					if (!variables.contains(key))
					{
						continue;
					}
				}

				variables.clear();
				args.erase(args.begin() + 1, args.begin() + 1 + ndispatch_keys);
				return opt.task(static_cast< int >(args.size()), args.data());
			}
			return 3;
		}
	};
}

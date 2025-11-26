export module main.task_2:processes_control;

import <print>;
import <iostream>;
import main.dispatch;
import :common;

namespace mains::task2
{
	export template< class Manager >
	void add_process(user_context< Manager >& ctx)
	{
		std::string name;
		std::uint64_t seed = 0;
		ctx.in >> name >> seed;
		if (!ctx.man.create_process(std::move(name), ctx.program,
					std::string{ctx.program} + ' ' + mains::dispatcher::executor_mark + ' ' + std::to_string(seed)))
		{
			std::println(ctx.err, "process with the same name already exists");
		}
	}
	export template< class Manager >
	void print_alive(user_context< Manager >& ctx)
	{
		size_t lives = 0;
		for (const std::string& name : ctx.man.process_names())
		{
			if (ctx.man.alive(name))
			{
				lives++;
				std::println(ctx.out, "{}", name);
			}
		}
		if (lives == 0)
		{
			std::println(ctx.out, "noone alive");
		}
	}
	export template< class Manager >
	void kill(user_context< Manager >& ctx)
	{
		using namespace std::chrono_literals;

		std::string name;
		ctx.in >> name;
		if (ctx.man.kill(name, ctx.kill_ping))
		{
			std::println(ctx.err, "hard kill \"{}\"", name);
		}
	}
}

export module main.task_2:processes_control;

import <print>;
import <iostream>;
import main.flags_parser;
import :common;

namespace mains::task2
{
	export template< class Manager >
	void create_process(user_context< Manager >& ctx)
	{
		std::string name;
		std::uint64_t seed = 0;
		ctx.in >> name >> seed;
		if (!ctx.man.create_process(std::move(name), ctx.program,
					std::format("{} {}-exec --tag={} {}", ctx.program, ctx.dispatch_key, name, seed)))
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
			std::println(ctx.out, "<no one alive>");
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
	export template< class Manager >
	void create_task(user_context< Manager >& ctx)
	{
		std::string process, tag, composition;
		std::size_t nthreads = 0;
		std::uint64_t ncycles = 0;
		ctx.in >> process >> tag >> composition >> nthreads >> ncycles;
		std::set< std::string >& comp = ctx.compositions.at(composition);
		auto [iter, ok] = ctx.tasks.try_emplace(tag, process);
		if (!ok)
		{
			std::println(ctx.err, "task with \"{}\" already exists", tag);
			return;
		}
		auto& stream = ctx.man.open_proc_stream(process);

		stream << nthreads << ncycles; // separators must be set by possibly-wrapped stream
		stream << comp.size();
		for (const std::string& i: comp)
		{
			stream << ctx.shapes.at(i);
		}

		ctx.man.send_out(stream);
	}
}

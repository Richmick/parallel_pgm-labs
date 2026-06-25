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
		if (!ctx.man.create_process(name, ctx.program,
					std::format("{} {}-exec --tag={} {} {}", ctx.program, ctx.dispatch_key, name,
						(Manager::traits::bin ? "--bin" : ""), seed)))
		{
			std::println(ctx.err, "process with the same name already exists");
			return;
		}
		ctx.proc_result_queues.try_emplace(name);
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
		std::size_t id = ctx.task_counter;
		auto [it, ok] = ctx.task_ids.try_emplace(tag, id);
		if (!ok)
		{
			std::println(ctx.err, "task with \"{}\" already exists", tag);
			return;
		}
		ctx.task_counter++;
		auto [iter, ok1] = ctx.tasks.try_emplace(id, process);

		auto& stream = ctx.man.open_proc_stream(process);
		stream << open_msg{};
		stream << id << nthreads << ncycles;
		stream << static_cast< std::size_t >(comp.size());
		for (const std::string& i: comp)
		{
			stream << ctx.shapes.at(i);
		}

		ctx.man.send_out(stream);
	}
	export template< class Manager >
	void await_task(user_context< Manager >& ctx)
	{
		std::string taskname;
		ctx.in >> taskname;
		std::size_t task_id = ctx.task_ids.at(taskname);
		calculation_task& task = ctx.tasks.at(task_id);
		auto& in = ctx.man.get_proc_in(task.executor);
		/*bool ok = wait_for_task(in, std::chrono::milliseconds{100}, task_id,
					ctx.proc_result_queues.at(task.executor), ctx.tasks);
		if (!ok)
		{
			std::println("<wait time limit exceeded>");
			return;
		}
		ctx.man.release_proc_in(in);
		std::println(ctx.out, "\"{}\" (#{}) returned {} after {}ms", taskname, task_id,
					task.result, task.nanoseconds / 1'000'000ULL);*/
		std::size_t id = 0;
		double res = 0.0;
		std::uint64_t dur;
		if (!(in >> id >> res >> dur))
		{
			std::println(ctx.err, "failed to read status");
			ctx.man.release_proc_in(in);
			return;
		}
		ctx.man.release_proc_in(in);
		std::println(ctx.out, "\"{}\" returned {} after {}ms", taskname, res, dur / 1'000'000ULL);
	}
}

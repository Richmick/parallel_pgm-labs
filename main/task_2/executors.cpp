module main.task_2;

import <exception>;
import <iostream>;
import <print>;

import square;
import parallel.thread;
import main.dispatch;

int mains::task2::stdin_executor(int argc, const char*const* argv)
{
	//mains::dispatcher disp{argc, argv};
	//auto event_p = disp.variables.at("--new-task-event"); // else no parallel tasks

	if (argc < 2)
	{
		std::println(std::cerr, "executor was started without seed");
		return 1;
	}
	std::uint64_t seed = 0;
	try
	{
		seed = std::stoull(argv[1]);
	}
	catch (const std::exception& e)
	{
		std::println(std::cerr, "executor failed to parse seed: {}", e.what());
		return 1;
	}

	square::composition task;
	{
		task.shapes.push_back(square::circle_t{1.0f, {0.0f, 0.0f}});
		square::settings set = {{{-1, -1}, {1, 1}}, 9, 100000000ULL, seed};
		try
		{
			std::uint64_t entries =
					square::count_entries(parallel::thread::std_manager< std::uint64_t >(set.nthreads - 1),
						set, square::in_shape{task});
			std::println("{:.4f}", square::square_tr(entries, set.whole_cycles, set.frame));
		}
		catch (const std::system_error& err)
		{
			std::println(std::cerr, "os error: {}", err.what());
			return 2;
		}
		task.shapes.resize(0);
	}

	std::string command;
	while (std::getline(std::cin, command))
	{
		std::cout << command;
	}
	return 0;
}

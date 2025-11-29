module main.task_2;

import <exception>;
import <iostream>;
import <print>;
import <span>;
import <set>;
import <variant>;
import <random>;

import square;
import parallel.thread;
import main.flags_parser;

int mains::task2::stdin_executor(int argc, const char*const* argv)
{
	mains::flags_parser flags{{argv + 1, static_cast< std::size_t >(argc - 1)}};
	std::string_view procname = flags["tag"];
	bool use_bin = flags.flags.contains("bin");

	if (argc < 2)
	{
		std::println(std::cerr, "[{}] executor was started without seed", procname);
		return 1;
	}
	std::uint64_t seed = 0;
	try
	{
		seed = std::stoull(argv[argc - 1]);
	}
	catch (const std::exception& e)
	{
		std::println(std::cerr, "[{}] executor failed to parse seed: {}", procname, e.what());
		return 1;
	}

	std::mt19937_64 rnd_eng{seed};
	square::composition composition;
	square::settings set;
	std::string open_tag;
	bool ignore_errors = false;
	std::println(std::clog, "[{}] initialized", procname);
	while (std::cin >> open_tag)
	{
		if (open_tag != "open")
		{
			if (!ignore_errors)
			{
				ignore_errors = true;
				std::println(std::cerr, "[{}] unexpected data in executor (await \"open\" signal)", procname);
			}
			continue;
		}
		ignore_errors = false;

		std::size_t comp_len = 0;
		if (!(std::cin >> set.nthreads >> set.whole_cycles >> comp_len))
		{
			std::println(std::cerr, "[{}] unexpected settings in executor", procname);
			continue;
		}
		composition.shapes.reserve(comp_len);
		for (; comp_len > 0; comp_len--)
		{
			if (!(std::cin >> open_tag))
			{
				std::println(std::cerr, "[{}] failed to read shape type", procname);
				continue;
			}
			if (open_tag == "circle")
			{
				square::circle_t c;
				if (!(std::cin >> c.radius >> c.center.x >> c.center.y))
				{
					std::println(std::cerr, "[{}] failed to read circle", procname);
					break;
				}
				composition.shapes.push_back(c);
			}
			else if (open_tag == "rectangle")
			{
				square::circle_t c;
				if (!(std::cin >> c.radius >> c.center.x >> c.center.y))
				{
					std::println(std::cerr, "[{}] failed to read rectangle", procname);
					break;
				}
				composition.shapes.push_back(c);
			}
			else
			{
				std::println(std::cerr, "[{}] unknown shape type", procname);
				break;
			}
		}
		if (comp_len != 0)
		{
			continue;
		}

		set.frame = square::get_frame::operator()(composition);
		set.seed = rnd_eng();
		std::println(std::clog, "[{}] start calculation", procname);
		try
		{
			std::uint64_t entries =
					square::count_entries(parallel::thread::std_manager< std::uint64_t >(set.nthreads - 1),
						set, square::in_shape{composition});
			std::println(std::cerr, "{:.4f}", square::square_tr(entries, set.whole_cycles, set.frame));
		}
		catch (const std::system_error& err)
		{
			std::println(std::cerr, "os error: {}", err.what());
			return 2;
		}
		composition.shapes.resize(0);
	}
	return 0;
}

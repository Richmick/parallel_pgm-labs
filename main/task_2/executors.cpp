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
import parallel.process;
import main.flags_parser;

namespace mains::task2
{
	class bin_input_switch
	{
	public:
		bin_input_switch(std::istream& s, bool bin):
			stream_(s)
		{}
		template< class T >
		bin_input_switch& operator>>(T& t)
		{
			if (binary_)
			{
				stream_ >> parallel::process::bin_pack< T >{t};
				return *this;
			}
			stream_ >> t;
			return *this;
		}
		std::istream& raw()
		{
			return stream_;
		}
		bool binary() const noexcept
		{
			return binary_;
		}
		operator bool()
		{
			return static_cast< bool >(stream_);
		}
	private:
		std::istream& stream_;
		bool binary_ = false;
	};
	bin_input_switch& operator>>(bin_input_switch& in, open_msg msg)
	{
		return in.operator>>(msg);
	}
}

int mains::task2::stdin_executor(int argc, const char*const* argv)
{
	mains::flags_parser flags{{argv + 1, static_cast< std::size_t >(argc - 1)}};
	std::string_view procname = flags["tag"];
	bool use_bin = flags.flags.contains("bin");
	bool make_logs = flags.flags.contains("log");

	bin_input_switch in{std::cin, use_bin};

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
	std::string strbuf;
	std::println(std::clog, "[{}] initialized", procname);
	while (in >> open_msg{})
	{
		std::size_t comp_len = 0;
		if (!(in >> set.nthreads >> set.whole_cycles >> comp_len) || (set.nthreads <= 0) || (comp_len <= 0))
		{
			std::println(std::cerr, "[{}] unexpected settings in executor", procname);
			continue;
		}
		composition.shapes.reserve(comp_len);
		for (; comp_len > 0; comp_len--)
		{
			if (!(std::cin >> strbuf))
			{
				std::println(std::cerr, "[{}] failed to read shape type", procname);
				continue;
			}
			if (strbuf == "circle")
			{
				square::circle_t c;
				if (!(std::cin >> c.radius >> c.center.x >> c.center.y))
				{
					std::println(std::cerr, "[{}] failed to read circle", procname);
					break;
				}
				if (c.radius <= 0)
				{
					std::println(std::cerr, "[{}] wrong circle radius", procname);
				}
				composition.shapes.push_back(c);
			}
			else if (strbuf == "rectangle")
			{
				square::rect_t r;
				if (!(std::cin >> r.p1.x >> r.p1.y >> r.p2.x >> r.p2.y))
				{
					std::println(std::cerr, "[{}] failed to read rectangle", procname);
					break;
				}
				composition.shapes.push_back(r);
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

module main.task_1;

import <istream>;
import <stdexcept>;
import <string>;

void mains::task1::parse_argv(int argc, const char*const* argv, square::settings& set)
{
	if ((argc < 2) || (argc > 3))
	{
		throw std::invalid_argument("wrong arguments count");
	}

	set.whole_cycles = std::stoull(argv[1]);
	if (argc >= 3)
	{
		set.seed = std::stoull(argv[2]);
	}
}
void mains::task1::parse_istream(std::istream& in, square::settings& set, float& radius)
{
	ptrdiff_t nthreads = 1;
	if (!(in >> radius >> nthreads))
	{
		throw std::runtime_error("failed to read test entries");
	}
	if (radius <= 0.0f)
	{
		throw std::invalid_argument("circle radius cannot be negative");
	}
	if (nthreads < 1)
	{
		throw std::invalid_argument("threads count cannot be less then 1");
	}

	set.nthreads = nthreads;
	set.frame = {{-radius, -radius}, {radius, radius}};
}

export module main.task_1;

import <iosfwd>;
import square;

export namespace mains
{
	namespace task1
	{
		int std_thread(int argc, const char*const* argv);
		int future_thread(int argc, const char*const* argv);
		int future_async(int argc, const char*const* argv);
		int os_dependent(int argc, const char*const* argv);
		// int boost(int argc, const char*const* argv);
		int algorithm(int argc, const char*const* argv);

		void parse_argv(int argc, const char*const* argv, square::settings& set);
		void parse_istream(std::istream& in, square::settings& set, float& radius);
	}
}

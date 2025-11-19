export module main.task_1;

import <iosfwd>;
import square;

export namespace mains
{
	namespace task1
	{
		int std_thread(int argc, char** argv);
		int future_thread(int argc, char** argv);
		int future_async(int argc, char** argv);
		int os_dependent(int argc, char** argv);
		// int boost(int argc, char** argv);
		int algorithm(int argc, char** argv);

		void parse_argv(int argc, char** argv, square::settings& set);
		void parse_istream(std::istream& in, square::settings& set, float& radius);
	}
}

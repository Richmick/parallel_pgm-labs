export module main.task_2:common;

import <chrono>;
import <iosfwd>;
import <string>;
import <map>;
import square;

namespace mains::task2
{
	template< class Manager >
	struct user_commands_parser
	{
		static constexpr inline std::chrono::milliseconds kill_ping{200};

		std::ostream& out;
		std::ostream& err;
		const char* program;

		std::map< std::string, square::composition::shape > shapes;
		std::map< std::string, square::composition > compositions;
		Manager man;

		void add_process(std::istream& in);
		void print_alive(std::istream& in);
		void kill(std::istream& in);
		void get_frame(std::istream& in);
		void get_set_frame(std::istream& in);
	};
}

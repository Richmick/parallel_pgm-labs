export module main.task_2:common;

import <chrono>;
import <iosfwd>;
import <string>;
import <map>;
import <set>;
import square;

namespace mains::task2
{
	export template< class Manager >
	struct user_context
	{
		static constexpr inline std::chrono::milliseconds kill_ping{200};

		std::istream& in;
		std::ostream& out;
		std::ostream& err;
		const char* program = nullptr;
		const char* dispatch_key = nullptr;

		std::map< std::string, square::composition::shape > shapes;
		std::map< std::string, std::set< std::string > > compositions;
		Manager man;
	};
}

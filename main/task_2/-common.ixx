export module main.task_2:common;

import <chrono>;
import <iosfwd>;
import <string>;
import <map>;
import <set>;
import square;
import parallel.process;

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
		std::map< std::string, std::string > tasks; // name:process
		Manager man;
	};
	export struct open_msg
	{};
}
export template<>
struct parallel::process::bin_pack< mains::task2::open_msg >
{
	mains::task2::open_msg msg;
};

namespace mains::task2
{
	export constexpr inline std::size_t open_msg_bin_length = 6;
	export parallel::process::char_stream_wrapper& operator<<(parallel::process::char_stream_wrapper& stream, open_msg)
	{
		return stream << "open";
	}
	std::istream& operator>>(std::istream& in, open_msg)
	{
		std::string open_tag;
		bool ignore_errors = false;
		while (in >> open_tag)
		{
			if (open_tag != "open")
			{
				std::println(std::cerr, "awaitopen - {}", open_tag);
				if (!ignore_errors)
				{
					ignore_errors = true;
					std::println(std::cerr, "unexpected data in executor (await \"open\" signal)");
				}
				continue;
			}
			break;
		}
		return in;
	}
	std::istream& operator>>(std::istream& in, parallel::process::bin_pack< open_msg >)
	{
		for (std::size_t i = 0; i < open_msg_bin_length;)
		{
			char byte = '\0';
			if (!(in >> byte))
			{
				return in;
			}
			(byte == ~(char)0 ? i++ : i = 0);
		}
		return in;
	}
}

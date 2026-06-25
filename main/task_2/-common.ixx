export module main.task_2:common;

import <chrono>;
import <iostream>;
import <string>;
import <map>;
import <set>;

import square;
import timer;
import parallel.process;

namespace mains::task2
{
	export struct calculation_task
	{
		std::string executor;
		std::size_t id = 0;
		double result = 0.0;
		std::uint64_t nanoseconds = 0;
		double multiplier = 1.0;
		signed char was_read = 0;
		bool last_was_digit = false;
	};
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
		std::map< std::string, std::size_t > task_ids;
		std::map< std::size_t, calculation_task > tasks;
		std::map< std::string, calculation_task > proc_result_queues;
		std::size_t task_counter = 0;
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
	export template< class S >
	struct task_waiter;
	export template<>
	struct task_waiter< parallel::process::char_stream_wrapper< std::istream > >
	{
		calculation_task& task;
		parallel::process::char_stream_wrapper< std::istream >& in;

		bool wait_for(std::chrono::milliseconds max_time)
		{
			chrono::timer timer;
			while ((timer.time_since_epoch() < max_time) && in)
			{
				if (task.was_read >= 3)
				{
					return true;
				}
				char buf;
				if (in.read_available(buf) == 1)
				{
					if ((task.was_read == 1) && (buf == '.') && (task.multiplier == 1.0))
					{
						task.multiplier = 0.1;
						continue;
					}
					if ((buf >= '0') && (buf <= '9'))
					{
						task.last_was_digit = true;
						switch (task.was_read)
						{
						case 0:
							task.id = task.id * 10 + (buf - '0');
							break;
						case 1:
							task.result = (task.multiplier == 1.0 ? task.result * 10 : task.multiplier)
									+ (buf - '0') * task.multiplier;
							if (task.multiplier != 1.0)
							{
								task.multiplier *= 0.1;
							}
							break;
						case 2:
							task.nanoseconds = task.nanoseconds * 10 + (buf - '0');
							break;
						}
						continue;
					}
					task.was_read += task.last_was_digit;
					task.last_was_digit = false;
				}
			}
			return false;
		}
	};
	export template<>
	struct task_waiter< parallel::process::bin_stream_wrapper< std::istream > >
	{
		calculation_task& task;
		parallel::process::bin_stream_wrapper< std::istream >& in;

		bool wait_for(std::chrono::milliseconds max_time)
		{
			chrono::timer timer;
			while ((timer.time_since_epoch() < max_time) && in)
			{
				char buf;
				if (in.read_available(buf) == 1)
				{
					task.was_read++;
					if ((task.was_read -= sizeof(calculation_task::id)) < 0)
					{
						reinterpret_cast<char*>(&task.id)[task.was_read] = buf;
					}
					else if ((task.was_read -= sizeof(calculation_task::result)) < 0)
					{
						reinterpret_cast< char* >(&task.result)[task.was_read] = buf;
					}
					else
					{
						reinterpret_cast<char*>(&task.nanoseconds)[task.was_read] = buf;
					}
				}
			}
			return false;
		}
	};
	export template< class S >
	bool wait_for_task(S& stream, std::chrono::milliseconds max_time, std::size_t required_id,
			calculation_task& buf, std::map< std::size_t, calculation_task >& tasks)
	{
		using std::chrono::milliseconds;
		using std::chrono::duration_cast;

		chrono::timer timer;
		task_waiter< S > waiter{buf, stream};
		while (waiter.wait_for(max_time - duration_cast< milliseconds >(timer.time_since_epoch())))
		{
			tasks.at(buf.id) = buf;
			if (buf.id == required_id)
			{
				buf = {};
				return true;
			}
			buf = {};
		}
		return false;
	}
	export constexpr inline std::size_t open_msg_bin_length = 6;
	export parallel::process::char_stream_wrapper< std::ostream >&
			operator<<(parallel::process::char_stream_wrapper< std::ostream >& stream, open_msg)
	{
		return stream << "open";
	}
	export parallel::process::bin_stream_wrapper< std::ostream >&
			operator<<(parallel::process::bin_stream_wrapper< std::ostream >& stream, open_msg)
	{
		for (std::size_t i = 0; i < open_msg_bin_length; i++)
		{
			stream << char(~0);
		}
		return stream;
	}
	std::istream& operator>>(std::istream& in, open_msg)
	{
		std::string open_tag;
		bool ignore_errors = false;
		while (in >> open_tag)
		{
			if (open_tag != "open")
			{
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

import <span>;
import <cstddef>;

import main.dispatch;
import main.task_1;
import main.task_2;

int main(int argc, const char*const* argv)
{
	static constexpr char thread_main_mark[] = "thread-main";
	static constexpr char process_main_mark[] = "process-main";

	static constexpr const char* shared_mem_vars[] = {"mem"};

	static constexpr mains::dispatch_option options[] = {
				{mains::task1::std_thread, thread_main_mark, "std", true},
				{mains::task1::future_thread, thread_main_mark, "std+future", true},
				{mains::task1::future_async, thread_main_mark, "std+async", true},
				{mains::task1::os_dependent, thread_main_mark, "os", true},
				{mains::task1::algorithm, thread_main_mark, "algorithm", true},

				{mains::task2::pipe, process_main_mark, "pipe", false},
				{mains::task2::stdin_executor, process_main_mark, "pipe-exec", true}
			};

	return mains::call_main({argv, std::size_t(argc)}, {options});
}

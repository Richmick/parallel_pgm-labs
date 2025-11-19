import <print>;
import <iostream>;
import <string_view>;
import <vector>;

import main.task_1;
import main.task_2;

int main(int argc, char** argv)
{
	constexpr char dispatch_thread_key[] = "--thread-main=";
	constexpr char dispatch_process_key[] = "--process-main=";
	if (argc < 2)
	{
		std::println(std::cerr, "there is no dispatch key");
		return 1;
	}
	std::vector< char* > pure_args(argv + 1, argv + argc);
	pure_args[0] = argv[0];
	if (std::string_view{argv[1]}.starts_with(dispatch_thread_key))
	{
		std::string_view name{argv[1] + sizeof(dispatch_thread_key) / sizeof(char) - 1};
		if (name == "std")
			return mains::task1::std_thread(argc - 1, pure_args.data());
		if (name == "std+future")
			return mains::task1::future_thread(argc - 1, pure_args.data());
		if (name == "std+async")
			return mains::task1::future_async(argc - 1, pure_args.data());
		if (name == "os")
			return mains::task1::os_dependent(argc - 1, pure_args.data());
		if (name == "algorithm")
			return mains::task1::algorithm(argc - 1, pure_args.data());

		std::println(std::cerr, "there is no implementation \"{}\"", name);
		return 3;
	}
	if (std::string_view{argv[1]} == mains::task2::executor_mark)
	{
		return mains::task2::common_executor(argc, argv);
	}
	if (std::string_view{argv[1]}.starts_with(dispatch_process_key))
	{
		std::string_view name{argv[1] + sizeof(dispatch_process_key) / sizeof(char) - 1};
		if (name == "pipe")
			return mains::task2::pipe_controller(argc - 1, pure_args.data());

		std::println(std::cerr, "there is no implementation \"{}\"", name);
		return 3;
	}

	std::println(std::cerr, "unknown test name");
	return 1;
}

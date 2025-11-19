export module main.task_2;

export namespace mains
{
	namespace task2
	{
		int pipe_controller(int argc, char** argv);
		int named_pipe_controller(int argc, char** argv);
		int shared_mem_controller(int argc, char** argv);

		int common_executor(int argc, char** argv);

		constexpr inline char executor_mark[] = "--executor";
	}
}

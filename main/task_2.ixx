export module main.task_2;

export namespace mains
{
	namespace task2
	{
		int pipe(int argc, char** argv);
		int named_pipe(int argc, char** argv);
		int shared_mem(int argc, char** argv);

		int stdin_executor(int argc, char** argv);
	}
}

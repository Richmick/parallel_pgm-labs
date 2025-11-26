export module main.task_2;

export import :common;
export import :shapes_cmds;
export import :processes_control;

export namespace mains::task2
{
	int pipe(int argc, char** argv);
	int named_pipe(int argc, char** argv);
	int shared_mem(int argc, char** argv);

	int stdin_executor(int argc, char** argv);
	int event_raw_executor(int argc, char** argv);
}

export module main.task_2;

export import :common;
export import :shapes_cmds;
export import :processes_control;

export namespace mains::task2
{
	int pipe(int argc, const char*const* argv);
	int named_pipe(int argc, const char*const* argv);
	int shared_mem(int argc, const char*const* argv);

	int stdin_executor(int argc, const char*const* argv);
	int event_raw_executor(int argc, const char*const* argv);
}

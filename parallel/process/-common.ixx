export module parallel.process:common;

namespace parallel::process
{
	export enum class control_policy
	{
		anonymous_tube,
		bin_anonymous_tube,
		named_tube,
		bin_named_tube,
		system_messages,
		copy_request
	};
	export enum class notify_policy
	{
		sysmsg,
		shared_memory,
		named_events,
		await_thread
	};
}

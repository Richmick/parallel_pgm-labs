module;
	#ifdef _WIN32
		#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
		#include <processthreadsapi.h>
	#endif
export module parallel.process:windows_impl;

#ifdef _WIN32

import <type_traits>;
import <string>;
import <string_view>;
import <iostream>;
import <map>;
import <ranges>;
import <chrono>;

import os.winapi;
import :common;
import :streams;

namespace parallel
{
	namespace process
	{
		export struct windows_pipe_data
		{
			winapi::unique_handle process_handle;
			pipe_buf< winapi::pipe > read_buf, write_buf;
			std::istream in{&read_buf};
			std::ostream out{&write_buf};
			char_stream_wrapper wrapped_out{out};
		};
		export template< control_policy ControlPol, notify_policy NotifyPol >
		struct windows_manager_traits
		{
			static constexpr inline bool anonymus_tube = (ControlPol == control_policy::anonymous_tube)
					|| (ControlPol == control_policy::bin_anonymous_tube);
			static constexpr inline bool named_tube = (ControlPol == control_policy::named_tube)
					|| (ControlPol == control_policy::bin_named_tube);
			static constexpr inline bool bin = (ControlPol == control_policy::bin_anonymous_tube)
					|| (ControlPol == control_policy::bin_named_tube);
			static constexpr inline bool tube = anonymus_tube || named_tube;
		};
		export template< control_policy ControlPol, notify_policy NotifyPol >
		class windows_manager
		{
		public:
			using traits = windows_manager_traits< ControlPol, NotifyPol >;

			static constexpr inline std::chrono::milliseconds close_lag{100};

			~windows_manager()
			{
				for (const auto& [name, info]: processes_)
				{
					try
					{
						kill(name, close_lag);
					}
					catch(...)
					{}
				}
			}
			bool create_process(std::string name, std::string_view executable, std::string params)
			{
				if (processes_.contains(name))
				{
					return false;
				}
				winapi::pipe results, commands;
				if constexpr (traits::anonymus_tube)
				{
					results = winapi::pipe{4096};
					commands = winapi::pipe{4096};
				}
				else
				{
					results = winapi::pipe{"proc_results__" + name};
					commands = winapi::pipe{"proc_commands__" + name};
				}
				winapi::unique_handle out_child = results.release_write();
				winapi::unique_handle in_child = commands.release_read();

				PROCESS_INFORMATION proc_info = {};
				STARTUPINFOA startup_info = {
					.cb = sizeof(startup_info),
					.dwFlags = STARTF_USESTDHANDLES,
					.hStdInput = in_child.get(),
					.hStdOutput = out_child.get(),
					.hStdError = GetStdHandle(STD_ERROR_HANDLE)
				};
				if (!CreateProcessA(executable.data(), params.data(), nullptr, nullptr, true,
							NORMAL_PRIORITY_CLASS, nullptr, nullptr, &startup_info, &proc_info))
				{
					throw winapi::error::last().exception("failed to create process");
				}
				winapi::unique_handle{proc_info.hThread};
				winapi::unique_handle created_proc{proc_info.hProcess};
				auto proc = processes_.try_emplace(std::move(name), std::move(created_proc),
								pipe_buf< winapi::pipe >{std::move(results)},
								pipe_buf< winapi::pipe >{std::move(commands)});
				pusher_.append(&proc.first->second.read_buf, &proc.first->second.write_buf);
				return true;
			}
			bool kill(const std::string& name, std::chrono::milliseconds close_time)
			{
				auto& proc = processes_.at(name);
				proc.write_buf.close();
				winapi::dword res = WaitForSingleObject(proc.process_handle.get(), static_cast< int >(close_time.count()));
				if (res == WAIT_OBJECT_0)
				{
					return false;
				}
				if (!TerminateProcess(proc.process_handle.get(), -1))
				{
					throw winapi::error::last().exception("failed to terminate process");
				}
				proc.process_handle.close();
				return true;
			}
			bool alive(const std::string& name)
			{
				winapi::unique_handle& proc = processes_.at(name).process_handle;
				if (!proc)
				{
					return false;
				}
				winapi::dword res = WaitForSingleObject(proc.get(), 0);
				if (res == WAIT_FAILED)
				{
					throw winapi::error::last().exception("failed to wait for process");
				}
				return res == WAIT_TIMEOUT;
			}
			auto process_names() const
			{
				return std::views::keys(processes_);
			}
			auto& open_proc_stream(const std::string& procname)
			{
				pusher_.lock();
				return processes_.at(procname).wrapped_out;
			}
			void send_out(auto& stream)
			{
				pusher_.unlock();
			}

		private:
			pipe_pusher< winapi::pipe > pusher_;
			std::map< std::string, windows_pipe_data > processes_;
		};
	}
}

#endif

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

import os.winapi;
import :common;

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
		};
		export template< control_policy ControlPol, notify_policy NotifyPol >
		class windows_manager
		{
		public:
			~windows_manager()
			{
				for (const auto& [name, info]: processes_)
				{
					try
					{
						kill(name);
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
				if constexpr (ControlPol == control_policy::anonymous_tube)
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
					throw winapi::error::last().exception();
				}
				winapi::unique_handle{proc_info.hThread};
				winapi::unique_handle created_proc{proc_info.hProcess};
				auto proc = processes_.try_emplace(std::move(name), std::move(created_proc),
								pipe_buf< winapi::pipe >{std::move(results)},
								pipe_buf< winapi::pipe >{std::move(commands)});
				proc.first->second.out << "hello\n";
				//proc.first->second.write_buf.sync();
				pusher_.append(&proc.first->second.read_buf, &proc.first->second.write_buf);
				return true;
			}
			void kill(const std::string& name)
			{
				winapi::unique_handle& proc = processes_.at(name).process_handle;
				if (!TerminateProcess(proc.get(), -1))
				{
					throw winapi::error::last().exception();
				}
				proc.close();
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
					throw winapi::error::last().exception();
				}
				return res == WAIT_TIMEOUT;
			}
			auto process_names() const
			{
				return std::views::keys(processes_);
			}
		private:
			struct empty
			{};
			std::conditional_t< has_pipe, pipe_pusher< winapi::pipe >, empty > pusher_;
			std::map< std::string, process_info > processes_;
		};
	}
}

#endif

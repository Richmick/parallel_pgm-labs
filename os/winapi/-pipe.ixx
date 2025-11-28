module;
	#ifdef _WIN32
		#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
	#endif
export module os.winapi:pipe;

#ifdef _WIN32

import <cstddef>;
import <string>;
import <utility>;
import <string>;

import :common;
import :unique_handle;
import :error;

namespace winapi
{
	export class pipe
	{
	public:
		pipe() noexcept = default;
		pipe(unique_handle read, unique_handle write) noexcept:
			read_{std::move(read)},
			write_{std::move(write)}
		{}
		explicit pipe(dword hint_size)
		{
			SECURITY_ATTRIBUTES attr = {
						.nLength = sizeof(SECURITY_ATTRIBUTES),
						.lpSecurityDescriptor = nullptr,
						.bInheritHandle = true
					};
			if (!CreatePipe(&read_, &write_, &attr, hint_size))
			{
				throw error::last().exception("failed to create pipe");
			}
		}
		explicit pipe(std::string name);

		bool can_read() const noexcept
		{
			return read_;
		}
		bool can_write() const noexcept
		{
			return write_;
		}
		unique_handle release_write() noexcept
		{
			return std::move(write_);
		}
		unique_handle release_read() noexcept
		{
			return std::move(read_);
		}
		winapi::handle_t get_write() noexcept
		{
			return write_.get();
		}
		winapi::handle_t get_read() noexcept
		{
			return read_.get();
		}
		std::size_t write_available(char* data, std::size_t len)
		{
			dword written = 0;
			if (!WriteFile(write_.get(), data, static_cast<dword>(len), &written, nullptr))
			{
				throw error::last().exception("failed to write to pipe");
			}
			return written;
		}
		std::size_t read_available(char* data, std::size_t len)
		{
			dword was_read = 0;
			if (!PeekNamedPipe(read_.get(), data, static_cast< dword >(len), &was_read, nullptr, nullptr))
			{
				throw error::last().exception("failed to read from file");
			}
			return was_read;
		}
	private:
		unique_handle read_, write_;
	};
}

#endif

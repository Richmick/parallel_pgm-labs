module;
	#ifdef _WIN32
		#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
	#endif
export module os.winapi:error;

#ifdef _WIN32

import <format>;
import <memory>;
import <string_view>;
import <system_error>;

import :common;

namespace winapi
{
	export class error
	{
	public:
		error(dword code):
			code_(code)
		{}

		static error last()
		{
			return {GetLastError()};
		}
		std::system_error exception()
		{
			return {static_cast< int >(code_), std::system_category()};
		}
		dword code() const noexcept
		{
			return code_;
		}
	private:
		dword code_;
	};
}
export template <>
struct std::formatter< winapi::error, char >: public std::formatter< std::string_view, char >
{
	std::format_context::iterator format(const winapi::error& err, std::format_context& ctx) const
	{
		char* ptr = nullptr;
		std::size_t len = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
					| FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, err.code(),
					MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
					reinterpret_cast< char* >(&ptr), 0, nullptr);
		std::unique_ptr< char, decltype(&LocalFree) > message = {ptr, LocalFree};
		return std::formatter< std::string_view >::format({ptr, (len < 2 ? len : len - 2)}, ctx);
	}
};

#endif

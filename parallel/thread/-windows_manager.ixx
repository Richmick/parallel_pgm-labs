module;
	#ifdef _WIN32
		#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
	#endif
export module parallel.thread:windows_manager;

#ifdef _WIN32
	import <concepts>;
	import <vector>;
	import <utility>;
	import <tuple>;
	import <memory>;
	import <system_error>;

	import os.winapi;
	import :common;
#endif

namespace parallel
{
	namespace thread
	{
		export template< class R >
		class windows_manager;
	}
}

#ifdef _WIN32

namespace parallel
{
	namespace thread
	{
		template< class R, class F, class... Args > requires(std::invocable< F, Args... >)
		unsigned __stdcall windows_call_wrap(void* data_p)
		{
			tuple_call_wrap< R, F, Args... >(data_p);
			return 0;
		}
	}
}

template< class R >
class parallel::thread::windows_manager
{
public:
	explicit windows_manager(std::size_t count):
		results_(count)
	{
		threads_.reserve(count);
	}
	template< class F, class... Args > requires(decay_invocable< F, Args... >)
	void push_thread(F&& func, Args&&... args)
	{
		using data_type = std::tuple< R&, std::decay_t< F >, std::decay_t< Args >... >;
		auto data = std::make_unique< data_type >(results_[threads_.size()], func, args...);

		winapi::unique_handle thread(reinterpret_cast< winapi::handle_t >(::_beginthreadex(nullptr, 0U,
							windows_call_wrap< R, std::decay_t< F >, std::decay_t< Args >... >,
							data.get(), 0, nullptr)));
		if (!thread)
		{
			throw winapi::error::last().exception();
		}
		threads_.push_back(std::move(thread));
		data.release();
	}
	R await_value(std::size_t idx)
	{
		std::uint32_t code = WaitForSingleObject(threads_[idx].get(), ~std::uint32_t{0});
		if (code != WAIT_OBJECT_0)
		{
			throw winapi::error::last().exception();
		}
		return results_[idx];
	}

private:
	std::vector< winapi::unique_handle > threads_;
	std::vector< R > results_;
};

#endif

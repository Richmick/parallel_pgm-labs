module;
	#ifdef __linux__
		#include <pthread.h>
	#endif
export module parallel.thread:linux_manager;

#ifdef __linux__
	import <concepts>;
	import <vector>;
	import <utility>;
	import <tuple>;
	import <memory>;
	import <system_error>;
	import <cerrno>;

	import :common;
#endif

namespace parallel
{
	namespace thread
	{
		export template< class R >
		class linux_manager;
	}
}

#ifdef __linux__

namespace parallel
{
	namespace thread
	{
		template< class R, class F, class... Args > requires(std::invocable< F, Args... >)
		void* linux_call_wrap(void* data_p)
		{
			tuple_call_wrap< R, F, Args... >(data_p);
			return nullptr;
		}
	}
}

template< class R >
class parallel::thread::linux_manager
{
public:
	explicit linux_manager(std::size_t count):
		results_(count)
	{
		threads_.reserve(count);
	}
	template< class F, class... Args > requires(decay_invocable< F, Args... >)
	void push_thread(F&& func, Args&&... args)
	{
		using data_type = std::tuple< R&, std::decay_t< F >, std::decay_t< Args >... >;
		auto data = std::make_unique< data_type >(results_[threads_.size()], func, args...);

		pthread_t thread;
		int code = pthread_create(&thread, nullptr,
					linux_call_wrap< R, std::decay_t< F >, std::decay_t< Args >... >, data.get());
		if (code != 0)
		{
			throw std::system_error(errno, std::system_category());
		}
		threads_.push_back(thread);
		data.release();
	}
	R await_value(std::size_t idx)
	{
		int code = pthread_join(threads_[idx], nullptr);
		if (code != 0)
		{
			throw std::system_error(errno, std::system_category());
		}
		return results_[idx];
	}

private:
	std::vector< pthread_t > threads_;
	std::vector< R > results_;
};

#endif

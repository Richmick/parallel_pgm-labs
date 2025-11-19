export module parallel.thread:standard_managers;

//import std;
import <functional>;
import <concepts>;
import <thread>;
import <vector>;
import <utility>;
import <future>;

import :common;

namespace parallel
{
	namespace thread
	{
		template< class R >
		struct return_write_call_wrap
		{
			template< class F, class... Args > requires(std::invocable< F, Args... >)
			inline void operator()(R& result, F&& f, Args&&... args);
		};

		export template< class R >
		class std_manager
		{
		public:
			explicit std_manager(std::size_t count);

			template< class F, class... Args > requires(decay_invocable< F, Args... >)
			void push_thread(F&& func, Args&&... args);
			R await_value(std::size_t idx);

		private:
			std::vector< std::thread > threads_;
			std::vector< R > results_;
		};

		export template< class R >
		class future_manager
		{
		public:
			explicit future_manager(std::size_t count);

			template< class F, class... Args > requires(decay_invocable< F, Args... >)
			void push_thread(F&& func, Args&&... args);
			R await_value(std::size_t idx);

		private:
			std::vector< std::pair< std::future< R >, std::thread > > threads_;
		};

		export template< class R >
		class future_async_manager
		{
		public:
			explicit future_async_manager(std::size_t count);

			template< class F, class... Args > requires(decay_invocable< F, Args... >)
			void push_thread(F&& func, Args&&... args);
			R await_value(std::size_t idx);

		private:
			std::vector< std::future< R > > threads_;
		};
	}
}
template< class R >
template< class F, class... Args > requires(std::invocable< F, Args... >)
inline void parallel::thread::return_write_call_wrap< R >::operator()(R& result, F&& f, Args&&... args)
{
	result = std::invoke(std::forward< F >(f), std::forward< Args >(args)...);
}

template< class R >
parallel::thread::std_manager< R >::std_manager(std::size_t count):
	results_(count)
{
	threads_.reserve(count);
}
template< class R >
template< class F, class... Args > requires(parallel::decay_invocable< F, Args... >)
void parallel::thread::std_manager< R >::push_thread(F&& func, Args&&... args)
{
	threads_.emplace_back(return_write_call_wrap< R >{},
				std::ref(results_[threads_.size()]), std::forward< F >(func), std::forward< Args >(args)...);
}
template< class R >
R parallel::thread::std_manager< R >::await_value(std::size_t idx)
{
	threads_[idx].join();
	return results_[idx];
}

template< class R >
parallel::thread::future_manager< R >::future_manager(std::size_t count)
{
	threads_.reserve(count);
}
template< class R >
template< class F, class... Args > requires(parallel::decay_invocable< F, Args... >)
void parallel::thread::future_manager< R >::push_thread(F&& func, Args&&... args)
{
	std::packaged_task task{std::forward< F >(func)};
	std::future< R > future = task.get_future();
	threads_.emplace_back(std::move(future),
				std::thread{std::move(task), std::forward< Args >(args)...});
}
template< class R >
R parallel::thread::future_manager< R >::await_value(std::size_t idx)
{
	threads_[idx].second.join();
	return threads_[idx].first.get();
}

template< class R >
parallel::thread::future_async_manager< R >::future_async_manager(std::size_t count)
{
	threads_.reserve(count);
}
template< class R >
template< class F, class... Args > requires(parallel::decay_invocable< F, Args... >)
void parallel::thread::future_async_manager< R >::push_thread(F&& func, Args&&... args)
{
	threads_.emplace_back(std::async(std::launch::async,
					std::forward< F >(func), std::forward< Args >(args)...));
}
template< class R >
R parallel::thread::future_async_manager< R >::await_value(std::size_t idx)
{
	threads_[idx].wait();
	return threads_[idx].get();
}

export module parallel.process:pipe_pusher;

import <mutex>;
import <thread>;
import <system_error>;
import <iostream>;
import <forward_list>;

import :pipe_buf;

namespace parallel::process
{
	export template< class Pipe >
	class pipe_pusher: public std::mutex
	{
	public:
		pipe_pusher():
			thread_(waiter, std::ref(*this))
		{}
		pipe_pusher(const pipe_pusher&) = delete;
		pipe_pusher(pipe_pusher&&) = delete;
		~pipe_pusher()
		{
			lock();
			continue_running_ = false;
			unlock();

			if (thread_.joinable())
			{
				thread_.join();
			}
		}

		void append(pipe_buf< Pipe >* read, pipe_buf< Pipe >* write)
		{
			std::lock_guard guard{*this};
			buffers_.emplace_front(typename decltype(buffers_)::value_type{read, write});
		}
	private:
		std::thread thread_;
		std::forward_list< std::array< pipe_buf< Pipe >*, 2 > > buffers_;
		volatile bool continue_running_ = true;

		static void waiter(pipe_pusher& pusher)
		{
			while (pusher.continue_running_)
			{
				std::this_thread::yield();
				std::lock_guard guard{pusher};
				if (!pusher.continue_running_)
				{
					return;
				}
				for (auto [read, write]: pusher.buffers_)
				{
					bool readed = false;
					try
					{
						read->sync();
						readed = true;
						write->sync();
					}
					catch (const std::system_error& e)
					{
						if (e.code() == std::errc::broken_pipe)
						{
							(readed ? write : read)->close();
						}
					}
					catch (const std::exception& e)
					{
						std::println(std::cerr, "pusher had cought an error: {}", e.what());
					}
				}
			}
		}
	};
}

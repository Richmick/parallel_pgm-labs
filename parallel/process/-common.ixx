export module parallel.process:common;

import <thread>;
import <forward_list>;
import <array>;
import <utility>;
import <sstream>;
import <exception>;
// import std;

namespace parallel
{
	namespace process
	{
		export enum class control_policy
		{
			anonymous_tube,
			named_tube,
			shared_memory,
			shared_deque,
			system_messages
		};
		export enum class notify_policy
		{
			sysmsg,
			await_thread
		};

		export template< class Pipe >
		class pipe_buf: public std::stringbuf
		{
		public:
			pipe_buf(Pipe&& p):
				std::stringbuf(std::ios_base::in | std::ios_base::out),
				pipe(std::move(p))
			{}
			pipe_buf(pipe_buf&&) noexcept = default;
			pipe_buf& operator=(pipe_buf&&) noexcept = default;
			virtual ~pipe_buf() = default;

			int sync() override
			{
				int buf_res = std::stringbuf::sync();
				if (buf_res != 0)
				{
					return buf_res;
				}

				if (pipe.can_read())
				{
					std::size_t len = epptr() - pptr();
					if (len == 0)
					{
						constexpr size_t default_chunk = 1024;
						char buf[default_chunk];
						xsputn(buf, pipe.read_available(buf, default_chunk));
					}
					else
					{
						std::size_t len = epptr() - pptr();
						pbump((int)pipe.read_available(pptr(), (int)len));
					}
				}
				else if (pipe.can_write())
				{
					std::size_t len = egptr() - gptr();
					gbump((int)pipe.write_available(gptr(), (int)len));
				}
				return 0;
			}
		protected:
			Pipe pipe;
		};
		export template< class Pipe >
		class pipe_pusher
		{
		public:
			pipe_pusher():
				thread_(waiter, std::ref(continue_running_), std::ref(buffers_))
			{}
			pipe_pusher(const pipe_pusher&) = delete;
			pipe_pusher(pipe_pusher&&) = delete;
			~pipe_pusher()
			{
				continue_running_ = false;
				if (thread_.joinable())
				{
					thread_.join();
				}
			}

			void append(pipe_buf< Pipe >* read, pipe_buf< Pipe >* write)
			{
				buffers_.emplace_front(typename decltype(buffers_)::value_type{read, write});
			}
		private:
			std::thread thread_;
			std::forward_list< std::array< pipe_buf< Pipe >*, 2 > > buffers_;
			volatile bool continue_running_ = true;

			static void waiter(volatile bool& continue_running, decltype(buffers_)& buffers)
			{
				while (continue_running)
				{
					// add WaitForMultipleObject if count < 64 (macro limit)
					for (auto [read, write]: buffers)
					{
						try
						{
							read->sync();
							write->sync();
							//std::cout << read->str();
						}
						catch (...)
						{
							// close pipes
						}
					}
					std::this_thread::yield();
				}
			}
		};
	}
}

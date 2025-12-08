export module parallel.process:common;

import <thread>;
import <forward_list>;
import <array>;
import <utility>;
import <sstream>;
import <exception>;
import <system_error>;
import <print>;
import <iostream>;
import <mutex>;
import <functional>;
//import std;

namespace parallel
{
	namespace process
	{
		export enum class control_policy
		{
			anonymous_tube,
			named_tube,
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

		export template< class Pipe >
		class pipe_buf: public std::stringbuf
		{
		public:
			pipe_buf(Pipe&& p):
				std::stringbuf(std::ios_base::in | std::ios_base::out),
				pipe_(std::move(p))
			{}
			pipe_buf(pipe_buf&&) noexcept = default;
			pipe_buf& operator=(pipe_buf&&) noexcept = default;
			virtual ~pipe_buf() = default;

			pipe_buf* open(Pipe p)
			{
				pipe_ = std::move(p);
				return this;
			}
			pipe_buf* close()
			{
				pipe_.release_read();
				pipe_.release_write();
				return this;
			}

			int sync() override
			{
				int buf_res = std::stringbuf::sync();
				if (buf_res != 0)
				{
					return buf_res;
				}

				if (pipe_.can_read())
				{
					std::size_t len = epptr() - pptr();
					if (len == 0)
					{
						constexpr size_t default_chunk = 1024;
						char buf[default_chunk];
						xsputn(buf, pipe_.read_available(buf, default_chunk));
					}
					else
					{
						std::size_t len = epptr() - pptr();
						pbump((int)pipe_.read_available(pptr(), (int)len));
					}
				}
				else if (pipe_.can_write())
				{
					underflow();
					std::size_t len = egptr() - gptr();
					gbump((int)pipe_.write_available(gptr(), (int)len));
				}
				return 0;
			}
		protected:
			Pipe pipe_;
		};
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

			static void waiter(pipe_pusher& pusher)
			{
				while (pusher.continue_running_)
				{
					std::lock_guard guard{pusher};
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
					std::this_thread::yield();
				}
			}
		};
	}
}

export module parallel.process:pipe_buf;

import <sstream>;

namespace parallel::process
{
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
}

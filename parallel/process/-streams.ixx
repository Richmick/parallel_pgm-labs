export module parallel.process:streams;

import <print>;
import <ostream>;
import <variant>;
import :bin_pack;

namespace parallel::process
{
	export template< class S >
	struct char_stream_wrapper
	{
		S& stream_;
		template< class T >
		char_stream_wrapper& operator<<(const T& i)
				requires(requires{ std::println(stream_, "{}", i); })
		{
			std::println(stream_, "{}", i);
			return *this;
		}
		template< class... Types >
		char_stream_wrapper& operator<<(const std::variant< Types... >& i)
				requires(requires(const Types&... types){ std::println(stream_, "{}", types...); })
		{
			std::visit([this](auto i){ std::println(stream_, "{}", i); }, i);
			return *this;
		}
		template< class T >
		char_stream_wrapper& operator>>(T& i)
				requires(requires{ stream_ >> i; })
		{
			stream_ >> i;
			return *this;
		}
		std::streamsize read_available(char& c)
				requires(requires{ stream_.readsome(&c, 1); })
		{
			return stream_.readsome(&c, 1);
		}
		bool operator!() { return !stream_; }
		operator bool() { return static_cast< bool >(stream_); }
	};
	export template< class S >
	struct bin_stream_wrapper
	{
		S& stream_;
		template< class T >
		bin_stream_wrapper& operator<<(const T& i)
				requires(requires{ stream_ << bin_pack{i}; })
		{
			stream_ << bin_pack{i};
			return *this;
		}
		template< class T >
		bin_stream_wrapper& operator>>(T& i)
				requires(requires{ stream_ >> i; })
		{
			stream_ >> bin_pack{i};
			return *this;
		}
		std::streamsize read_available(char& c)
				requires(requires{ stream_.readsome(&c, 1); })
		{
			return stream_.readsome(&c, 1);
		}
		bool operator!() { return !stream_; }
		operator bool() { return static_cast< bool >(stream_); }
	};
}

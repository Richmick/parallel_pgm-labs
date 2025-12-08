export module parallel.process:streams;

import <print>;
import <ostream>;
import <variant>;
import :bin_pack;

namespace parallel::process
{
	export struct char_stream_wrapper
	{
		std::ostream& out;
		char_stream_wrapper& operator<<(const auto& i)
		{
			std::println(out, "{}", i);
			return *this;
		}
		template< class... Types >
		char_stream_wrapper& operator<<(const std::variant< Types... >& i)
		{
			std::visit([this](auto i){ std::println(out, "{}", i); }, i);
			return *this;
		}
	};
	export struct bin_stream_wrapper
	{
		std::ostream& out;
		bin_stream_wrapper& operator<<(const auto& i)
		{
			out << bin_pack{i};
			return *this;
		}
		template< class... Types >
		bin_stream_wrapper& operator<<(const std::variant< Types... >& i)
		{
			operator<<(i.index());
			std::visit([this](auto i){ operator<<(i); }, i);
			return *this;
		}
	};
}

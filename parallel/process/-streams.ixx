export module parallel.process:streams;

import <print>;
import <ostream>;
import <variant>;

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
}

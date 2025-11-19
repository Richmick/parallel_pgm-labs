export module parallel.thread;

export import :common;
export import :standard_managers;
export import :windows_manager;
export import :linux_manager;

import <type_traits>;
import os.basic;

namespace parallel
{
	namespace thread
	{
		export template< class R >
		using os_manager = std::conditional_t< os::family == os::type::linux, linux_manager< R >,
					std::conditional_t< os::family == os::type::windows, windows_manager< R >,
						std_manager< R > > >;
	}
}

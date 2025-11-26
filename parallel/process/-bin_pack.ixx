export module parallel.process:bin_pack;

import <iostream>;
import <type_traits>;

namespace parallel::process
{
	export template< class T > requires(std::is_trivially_destructible_v< T >)
	struct bin_pack
	{
		T& wrapped;
	};
	export template< class T >
	std::ostream& operator<<(std::ostream& stream, const bin_pack< T >& pack)
	{
		const char* raw = reinterpret_cast< const char* >(&pack.wrapped);
		for (std::size_t i = 0; i < sizeof(T); i++)
		{
			stream << raw[i];
		}
		return stream;
	}
	export template< class T >
	std::istream& operator>>(std::istream& stream, bin_pack< T >& pack)
	{
		char* raw = reinterpret_cast< char* >(&pack.wrapped);
		for (std::size_t i = 0; i < sizeof(T); i++)
		{
			stream >> raw[i];
		}
		return stream;
	}
}

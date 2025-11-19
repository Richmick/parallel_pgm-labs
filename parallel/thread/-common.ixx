export module parallel.thread:common;

//import std;
import <concepts>;
import <type_traits>;
import <functional>;
import <tuple>;
import <memory>;

namespace parallel
{
	export template< class F, class... Args >
	concept decay_invocable = std::invocable< std::decay_t< F >, std::decay_t< Args >... >;

	namespace thread
	{
		#if __cpp_structured_bindings < 202411L
			export template< class R, class F, class... Args, size_t... Idxs >
			R invoke_from_tuple(std::tuple< R&, F, Args... >& data, std::index_sequence< Idxs... >)
			{
				return std::invoke(std::get< Idxs + 1 >(data)...);
			}
		#endif
		export template< class R, class F, class... Args > requires(std::invocable< F, Args... >)
		void tuple_call_wrap(void* data_p)
		{
			using data_type = std::tuple< R&, F, Args... >;

			std::unique_ptr< data_type > data(reinterpret_cast< data_type* >(data_p));
			#if __cpp_structured_bindings >= 202411L
				auto&&[res, func, ...args] = *data;
				res = std::invoke(std::forward< F >(func), std::forward< Args >(args)...);
			#else
				std::get< 0 >(*data) = invoke_from_tuple(*data,
							std::make_index_sequence< std::tuple_size_v< data_type > - 1 >());
			#endif
		}
	}
}

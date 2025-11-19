export module os.basic;

namespace os
{
	export enum class type
	{
		linux,
		windows,
		other
	};
	#ifdef _WIN32
		export constexpr inline type family = type::windows;
		export constexpr const char family_name[] = "windows";
	#else
		#ifdef __linux__
			export constexpr inline type family = type::linux;
			export constexpr const char family_name[] = "linux";
		#else
			export constexpr inline type family = type::other;
			export constexpr const char family_name[] = "other";
		#endif
	#endif
}

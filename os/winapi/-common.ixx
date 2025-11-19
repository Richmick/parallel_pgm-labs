module;
	#ifdef _WIN32
		#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
	#endif
export module os.winapi:common;

#ifdef _WIN32
namespace winapi
{
	export using handle_t = HANDLE;
	export using dword = DWORD;
}
#endif
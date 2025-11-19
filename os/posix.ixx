module;
	#ifdef __linux__
		#include <unistd.h>
	#endif
export module os.posix;

#ifdef __linux__

import <utility>;
import <memory>;
import <system_error>;
import <cerrno>;

namespace posix
{
	export using descriptor_t = int;
	export class unique_descriptor
	{
	public:
		unique_descriptor() noexcept = default;
		explicit unique_descriptor(descriptor_t desc) noexcept:
			native_(desc)
		{}
		unique_descriptor(const unique_descriptor& src) = delete;
		unique_descriptor(unique_descriptor&& src) noexcept:
			native_(src.release())
		{}
		~unique_descriptor()
		{
			close();
		}
		unique_descriptor& operator=(const unique_descriptor& rhs) = delete;
		unique_descriptor& operator=(unique_descriptor&& rhs) noexcept
		{
			close();
			native_ = rhs.release();
			return *this;
		}

		void close()
		{
			if (operator bool())
			{
				::close(release());
			}
		}
		descriptor_t get() const noexcept
		{
			return native_;
		}
		descriptor_t release() noexcept
		{
			return std::exchange(native_, -1);
		}
		void swap(unique_descriptor& rhs) noexcept
		{
			std::swap(native_, rhs.native_);
		}

		descriptor_t* operator&()
		{
			close();
			return &native_;
		}
		operator bool() const noexcept
		{
			return native_ < 0;
		}
	private:
		int native_ = -1;
	};
}
namespace std
{
	export void swap(posix::unique_descriptor& lhs, posix::unique_descriptor& rhs) noexcept
	{
		lhs.swap(rhs);
	}
}

#endif

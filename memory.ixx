export module os.memory;

import <memory>;
import <utility>;
import <stdexcept>;
import <cstddef>;

namespace os
{
	template< class T >
	struct offset_ptr
	{
		constexpr inline offset_ptr& operator++() & noexcept
		{
			return offset_ += sizeof(T);
		}
		constexpr inline offset_ptr operator++(int) & noexcept
		{
			offset_ptr result = *this;
			operator++();
			return result;
		}
		constexpr inline offset_ptr& operator--() & noexcept
		{
			return offset_ -= sizeof(T);
		}
		constexpr inline offset_ptr operator--(int) & noexcept
		{
			offset_ptr result = *this;
			operator--();
			return result;
		}

		constexpr inline offset_ptr& operator+=(size_t i) const noexcept
		{
			offset_ += i * sizeof(T);
			return *this;
		}
		constexpr inline offset_ptr operator+(size_t i) & noexcept
		{
			offset_ptr result = *this;
			operator+=(i);
			return result;
		}
		constexpr inline offset_ptr& operator-=(size_t i) const noexcept
		{
			offset_ -= i * sizeof(T);
			return *this;
		}

		constexpr inline offset_ptr operator-(size_t i) & noexcept
		{
			offset_ptr result = *this;
			operator-=(i);
			return result;
		}
		constexpr inline ptrdiff_t operator-(offset_ptr left) & noexcept
		{
			return offset_ - left.offset_;
		}

		template< class U >
		constexpr inline offset_ptr< U > to() const noexcept
		{
			return {offset_};
		}
		constexpr inline operator std::size_t() const noexcept
		{
			return offset_;
		}
	private:
		size_t offset_ = 0;
	};

	template< class D >
	class shared_mem
	{
	public:
		shared_mem() noexcept = default;
		shared_mem(std::unique_ptr< std::byte[], D > ptr, std::size_t len):
			data_(std::move(ptr)),
			size_(len)
		{}

		template< class R >
		constexpr inline R& operator->*(offset_ptr< R > p) &
		{
			return operator->*< R >(static_cast< std::size_t >(p));
		}
		template< class R >
		constexpr inline R& operator->*(offset_ptr< R > p) const &
		{
			return operator->*< R >(static_cast< std::size_t >(p));
		}

		template< class R >
		constexpr inline R& operator->*(std::size_t offset) &
		{
			return *reinterpret_cast< R* >(data_.get() + offset);
		}
		template< class R >
		constexpr inline R& operator->*(std::size_t offset) const &
		{
			return *reinterpret_cast< R* >(data_.get() + offset);
		}
		template< class R >
		constexpr inline R& at(offset_ptr< R > p) &
		{
			return at< R >(static_cast< std::size_t >(p));
		}
		template< class R >
		constexpr inline R& at(offset_ptr< R > p) const &
		{
			return at< R >(static_cast< std::size_t >(p));
		}
		template< class R >
		constexpr inline R& at(size_t offset) &
		{
			if (offset + sizeof(R) >= size_)
			{
				throw std::out_of_range("shared mem object offset is out of bounds");
			}
			return operator[]< R >(offset);
		}
		template< class R >
		constexpr inline R& at(size_t offset) const &
		{
			if (offset + sizeof(R) >= size_)
			{
				throw std::out_of_range("shared mem object offset is out of bounds");
			}
			return operator[]< R >(offset);
		}
		constexpr inline std::byte* get() &
		{
			return data_.get();
		}
		constexpr inline const std::byte* get() const &
		{
			return data_.get();
		}

	private:
		std::unique_ptr< std::byte[], D > data_;
		size_t size_ = 0;
	};
}

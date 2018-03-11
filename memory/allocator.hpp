#include <memory>


namespace memory {


template <typename T, typename Allocator = std::allocator<T>>
struct without_value_initialization : Allocator { // https://stackoverflow.com/a/21028912
	using Allocator::Allocator;
	using traits = std::allocator_traits<Allocator>;

	template <typename U> struct rebind {
		using other = without_value_initialization<U, typename traits::template rebind_alloc<U>>;
	};

	template <typename U>
	void construct(U * address) noexcept(std::is_nothrow_default_constructible<U>::value) {
		::new (static_cast<void *>(address)) U;
	}

	template <typename U, typename... Args>
	void construct(U * address, Args&&... args) {
		traits::construct(static_cast<Allocator&>(*this), address, std::forward<Args>(args)...);
	}
};


}




#pragma once

#include <type_traits>
#include <algorithm>
#include <memory>


namespace memory {

template <typename T, typename Allocator = std::allocator<T>>
class shared : Allocator {
	auto& allocator() { return static_cast<Allocator&>(*this); }
	auto deleter() { return [this] (T * array) { allocator().deallocate(array, capacity); }; }
	std::shared_ptr<T> shared_ptr;
	size_t count = 0, capacity = 0;

public:
	template <typename InputIterator>
	shared(InputIterator start, InputIterator stop) : shared(std::distance(start, stop)) { std::copy(start, stop, begin()); }
	shared(size_t count) : shared_ptr(allocator().allocate(count), deleter(), allocator()), count(count), capacity(count) { }
	shared() : shared_ptr(nullptr, deleter(), allocator()), count(0), capacity(0) { }
	size_t size() const { return count; }

	template <typename... Args>
	void emplace_back(Args&&... args) {
		if (__builtin_expect(count == capacity, false)) {
			auto resize = std::max<size_t>(16, capacity * 2);
			auto replacement = std::shared_ptr<T>(allocator().allocate(resize), deleter(), allocator());
			std::copy_n(std::make_move_iterator(shared_ptr.get()), count, replacement.get());
			shared_ptr = std::move(replacement);
			capacity = resize;
		}

		allocator().construct(shared_ptr.get() + count++, std::forward<Args>(args)...);
	}

	T const& operator[](size_t index) const { return shared_ptr.get()[index]; }
	auto begin() const { return shared_ptr.get(); }
	auto end() const { return shared_ptr.get() + count; }
	T const& back() const { return end()[-1]; }
};


} // end namespace



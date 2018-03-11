#pragma once

#include "memory/shared.hpp"

#include <type_traits>
#include <string>


namespace memory {

template <typename T, typename Allocator = std::allocator<T>, typename Base = std::integral_constant<size_t, 4>>
class bagwell : Allocator {
	static constexpr size_t Initial = 1 << Base();
	auto& allocator() { return static_cast<Allocator&>(*this); }
	memory::shared<T*, typename Allocator::template rebind<T*>::other> indirection;
	T *next = nullptr, *last = nullptr;

public:
	template <typename InputIterator>
	bagwell(InputIterator start, InputIterator stop) { while (start != stop) emplace_back(*start++); }
	bagwell() = default;

	~bagwell() noexcept {
		for (size_t i = 0; i < indirection.size(); ++i) {
			if (auto array = indirection[i]; array)
				allocator().deallocate(array, Initial << i);
			else
				break;
		}
	}

	template <typename... Args> inline
	void __attribute__((always_inline)) emplace_back(Args&&... args) {
		if (__builtin_expect(next == last, false)) {
			[&] () __attribute__((noinline)) {
				size_t next_size = Initial << indirection.size();
				indirection.emplace_back(allocator().allocate(next_size));
				next = indirection.back();
				last = next + next_size;
			}();
		}

		allocator().construct(next++, std::forward<Args>(args)...);
	}

	inline T const& __attribute__((always_inline)) operator[](size_t index) const noexcept {
		const size_t fixed = index+Initial;
		auto fixed_zeroes = __builtin_clzll(fixed);
		const size_t msb_diff = (sizeof(size_t) * 8 - Base() - 1) - fixed_zeroes;
		const size_t msbit = 1 << ((sizeof(size_t) * 8 - 1) - fixed_zeroes);
		const size_t fixed₂ = fixed & ~msbit;
		return indirection[msb_diff][fixed₂];
	}

	size_t size() const noexcept {
		if (__builtin_expect(indirection.size() == 0, false))
			return 0;

		auto count = std::distance(indirection.back(), next);
		if (auto full = indirection.size() - 1; full != 0)
			count += Initial * ((1 << full) - 1);

		return count;
	}

	bool empty() const noexcept {
		return indirection.empty();
	}

public:
	auto begin(size_t n) const { return indirection[n]; }
	auto end(size_t n) const { return begin(n) + bucket_size(n); }
	auto bucket_count() const { return indirection.size(); }
	size_t bucket_size(size_t n) const {
		if (n < indirection.size() - 1)
			return Initial << n;

		return std::distance(indirection.back(), next);
	}

};


} // end namespace



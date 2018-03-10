#pragma once

#include "memory/shared.hpp"

#include <type_traits>


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

		allocator().construct(next, std::forward<Args>(args)...);
//		*next++ = T{args...};
		++next;
	}

	size_t size() const noexcept {
		if (__builtin_expect(indirection.size() == 0, false))
			return 0;

		auto count = std::distance(indirection.back(), next);
		if (auto full = indirection.size() - 1; full != 0) {
			count += Initial * ((1 << full) - 1);
		}

		return count;
	}

	inline T const& __attribute__((always_inline)) operator[](size_t index) const noexcept {
//		const unsigned long fixed = index+Initial;
//		int fixed_zeroes = __builtin_clzl(fixed);
//		const unsigned long msb_diff = (sizeof(unsigned long)*8-Base()-1) - fixed_zeroes;
//		const unsigned long msbit = 1 << ((sizeof(unsigned long)*8-1) - fixed_zeroes);
//		const unsigned long fixed₂ = fixed & ~msbit;
//
		const size_t fixed = index+Initial;
		auto fixed_zeroes = __builtin_clzll(fixed);
		const size_t msb_diff = (sizeof(size_t) * 8 - Base() - 1) - fixed_zeroes;
		const size_t msbit = 1 << ((sizeof(size_t) * 8 - 1) - fixed_zeroes);
		const size_t fixed₂ = fixed & ~msbit;
		return indirection[msb_diff][fixed₂];
	}

	template <class Function>
	friend void spans(bagwell const& container, Function&& function) {
		if (__builtin_expect(container.indirection.size() == 0, false))
			return;

		for (size_t i = 0; i < container.indirection.size() - 1; ++i)
			function(container.indirection[i], container.indirection[i] + (Initial << i));

		function(container.indirection.back(), container.next);
	}

	friend auto dot(bagwell const& container) {
		std::string graphviz;
		graphviz += "digraph G {\n";
		graphviz += "\tnode [shape=record, width=.1,height=.1];\n";
		graphviz += "\tedge [headport=w, tailport=e, arrowsize=0.5];\n";
		graphviz += "\trankdir=LR;\n";
		graphviz += "\tranksep=1;\n\n";

		graphviz += "\tindirection [label = \"";
		for (size_t i = 0; i < container.indirection.size(); ++i)
			graphviz += "<f" + std::to_string(i) + "> |";
		graphviz.pop_back();
		graphviz += " \"];\n";

		spans(container, [&, i = 0] (auto begin, auto end) mutable {
			graphviz += "\tspan" + std::to_string(i) + " [label = \"{<n>";
			std::for_each(begin, end, [&] (auto n) {
				graphviz += " " + std::to_string(n) + " |";
			});
			graphviz.pop_back();
			graphviz += "}\"];\n";

			graphviz += "\tindirection:f" + std::to_string(i) + " -> span" + std::to_string(i) + ":n;\n\n";
			++i;
		});

		graphviz += "}\n";
		return graphviz;
	}

};



} // end namespace



#include "memory/iterators.hpp"
#include "memory/shared.hpp"
#include "memory/bagwell.hpp"

#include <algorithm>
#include <numeric>
#include <vector>

#include "rapidcheck.h"


int main()
{
	rc::check("memory::shared[]", [ ] (std::vector<int> const& vector) {
		memory::shared<int> shared{std::begin(vector), std::end(vector)};
		for (size_t i = 0; i < vector.size(); ++i)
			RC_ASSERT(shared[i] == vector[i]);
	});

	rc::check("memory::bagwell[]", [ ] (std::vector<int> const& vector) {
		memory::bagwell<int> bagwell{std::begin(vector), std::end(vector)};
		for (size_t i = 0; i < vector.size(); ++i)
			RC_ASSERT(bagwell[i] == vector[i]);
	});

	rc::check("memory::shared — std::begin() std::end()", [ ] (std::vector<int> const& vector) {
		auto check = std::accumulate(std::begin(vector), std::end(vector), size_t{});

		memory::shared<int> shared{std::begin(vector), std::end(vector)};
		auto sum = std::accumulate(std::begin(shared), std::end(shared), size_t{});
		RC_ASSERT(sum == check);
	});

	rc::check("memory::bagwell — std::begin() std::end()", [ ] (std::vector<int> const& vector) {
		auto check = std::accumulate(std::begin(vector), std::end(vector), size_t{});

		memory::bagwell<int> bagwell{std::begin(vector), std::end(vector)};
		auto sum = std::accumulate(std::begin(bagwell), std::end(bagwell), size_t{});
		RC_ASSERT(sum == check);
	});

}



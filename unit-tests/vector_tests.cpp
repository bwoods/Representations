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
		memory::shared<int> shared{vector.begin(), vector.end()};
		for (size_t i = 0; i < vector.size(); ++i)
			RC_ASSERT(shared[i] == vector[i]);
	});

	rc::check("memory::bagwell[]", [ ] (std::vector<int> const& vector) {
		memory::bagwell<int> bagwell{vector.begin(), vector.end()};
		for (size_t i = 0; i < vector.size(); ++i)
			RC_ASSERT(bagwell[i] == vector[i]);
	});

	rc::check("memory::bagwell — std::begin() std::end()", [ ] (std::vector<int> const& vector) {
		auto check = std::accumulate(vector.begin(), vector.end(), size_t{});

		memory::bagwell<int> bagwell{vector.begin(), vector.end()};
		auto sum = std::accumulate(std::begin(bagwell), std::end(bagwell), size_t{});
		RC_ASSERT(sum == check);
	});

}



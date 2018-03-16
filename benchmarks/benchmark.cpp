#include "benchmark.hpp"

#include "memory/iterators.hpp"
#include "memory/bagwell.hpp"
#include "memory/shared.hpp"

#include <vector>


using namespace benchmarking;

int main()
{
	using type = int;

	benchmark<
		nonius::html_reporter, std::vector<type>, memory::bagwell<type>, memory::shared<type>
	>::suite("benchmark-{$test}-{$suffix}.html", { /*100000000,*/ 1000000, 10000, 1000, 100, 10 },
		"spans", [] (auto&& container, size_t count, nonius::chronometer& meter) __attribute__((noinline)) { // allowing inline ⇒ “nonius::benchmark_user_error: a benchmark failed to run successfully”
			fill_n(container, count);

			uint64_t sum = 0;
			meter.measure([&] {
				memory::spans(container, [&] (auto n) {
					sum += n;
				});
			});

			return sum;
		},
		"construction", [] (auto&& container, size_t count, nonius::chronometer& meter) __attribute__((noinline)) { // allowing inline ⇒ “nonius::benchmark_user_error: a benchmark failed to run successfully”
			std::vector<nonius::storage_for<std::decay_t<decltype(container)>>> storage(meter.runs());
			std::vector<size_t> values;
			fill_n(values, count);

			meter.measure([&] (size_t i) {
				storage[i].construct(values.data(), values.data() + values.size());
			});

			return container.size();
		},
		"emplace_back", [] (auto&& container, size_t count, nonius::chronometer& meter) __attribute__((noinline)) { // allowing inline ⇒ “nonius::benchmark_user_error: a benchmark failed to run successfully”
			std::vector<nonius::storage_for<std::decay_t<decltype(container)>>> storage(meter.runs());
			std::vector<size_t> values;
			fill_n(values, count);

			meter.measure([&] (size_t i) {
				auto& container = storage[i];
				container.construct();
				for (auto value : values)
					container.stored_object().emplace_back(value);
			});

			return container.size();
		}
	);
}

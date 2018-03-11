#define NDEBUG 1
//#define NONIUS_USE_BOOST_CHRONO 1
#include "nonius/nonius.h++"

#include "memory/allocator.hpp"
#include "memory/iterators.hpp"
#include "memory/shared.hpp"
#include "memory/bagwell.hpp"

#include <iterator>
#include <vector>
#include <string>


template <template <typename...> class Container>
auto __attribute__((noinline)) zero_to(size_t n) { // `noinline` stops the optimizer from computing the sum at compile-time
	std::vector<int, memory::without_value_initialization<int>> vec(n);
	std::iota(std::begin(vec), std::end(vec), 0);
	return Container<int, memory::without_value_initialization<int>>{vec.data(), vec.data() + n};
}

template <template <typename...> class Container>
struct construction {
	auto __attribute__((noinline)) operator() (size_t count, nonius::chronometer& meter) {
		auto values = zero_to<std::vector>(count);
		std::vector<nonius::storage_for<decltype(zero_to<Container>(count))>> storage(meter.runs());

		meter.measure([&] (size_t i) {
			storage[i].construct(values.data(), values.data() + values.size());
		});
	}
};

template <template <typename...> class Container>
struct iteration {
	auto __attribute__((noinline)) operator() (size_t count, nonius::chronometer& meter) {
		uint64_t sum = 0;
		auto values = zero_to<Container>(count);

		meter.measure([&] {
			sum = std::accumulate(std::begin(values), std::end(values), size_t{});
		});
		return sum;
	}
};

template <template <typename...> class Container>
struct linear_indexing {
	auto __attribute__((noinline)) operator() (size_t count, nonius::chronometer& meter) {
		uint64_t sum = 0;
		auto values = zero_to<Container>(count);

		meter.measure([&] {
			sum = 0;
			for(size_t i = 0; i < values.size(); ++i)
				sum += values[i];
		});
		return sum;
	}
};

template <template <typename...> class Container>
struct random_indexing {
	auto __attribute__((noinline)) operator() (size_t count, nonius::chronometer& meter) {
		uint64_t sum = 0;
		auto values = zero_to<Container>(count);
		auto indexes = zero_to<std::vector>(values.size());
		random_shuffle(std::begin(indexes), std::begin(indexes));

		meter.measure([&] {
			sum = 0;
			for(size_t i = 0; i < indexes.size(); ++i)
				sum += values[indexes[i]];
		});
		return sum;
	}
};

static unsigned iframe = 0;
template <template <template <typename...> class Container> class Measure>
auto benchmark(std::string title, size_t count) {
	nonius::configuration cfg;
	cfg.samples = 50;

	cfg.output_file = "fig-" + std::to_string(++iframe) + ".html";
	cfg.title = title + " — " + std::to_string(count);
	cfg.verbose = true;

	nonius::benchmark benchmarks[] = {
//		nonius::benchmark("warmup", [=] (nonius::chronometer meter) {
//			return Measure<std::vector>{}(count, meter);
//		}),
		nonius::benchmark("memory::shared", [=] (nonius::chronometer meter) {
			return Measure<memory::shared>{}(count, meter);
		}),
		nonius::benchmark("memory::bagwell", [=] (nonius::chronometer meter) {
			return Measure<memory::bagwell>{}(count, meter);
		}),
		nonius::benchmark("std::deque", [=] (nonius::chronometer meter) {
			return Measure<std::deque>{}(count, meter);
		}),
		nonius::benchmark("std::vector", [=] (nonius::chronometer meter) {
			return Measure<std::vector>{}(count, meter);
		}),
	};

	nonius::go(cfg, std::begin(benchmarks), std::end(benchmarks), nonius::html_reporter());
	return cfg.output_file;
}



#include <fstream>

static std::string html_template = R"(
<html>
<head>
</head>
<body>
{% for figure in figures %}<embed src='{$figure}' width='100%' height='440'>
{% endfor %}
</body>
</html>
)";
void report(std::vector<std::string> const& files) {
	cpptempl::data_list figures;
	for (auto& file : files)
		figures.push_back(file);

	cpptempl::data_map map;
	map["figures"] = cpptempl::make_data(figures);

	std::ofstream stream("benchmarks.html", std::ios::binary);
	cpptempl::parse(stream, html_template, map);
	stream << std::flush;
}


int main()
{
	size_t counts[] = { 50000000, 10000000, 1000000, 100000, 10000, 1000, 100, 10 };

	std::vector<std::string> files;
	for (auto count : counts) files.emplace_back(benchmark<construction>("Construction", count));
	for (auto count : counts) files.emplace_back(benchmark<random_indexing>("Random Indexing", count));
	for (auto count : counts) files.emplace_back(benchmark<linear_indexing>("Linear Indexing", count));
	for (auto count : counts) files.emplace_back(benchmark<iteration>("Iterators", count));

	report(files);

}



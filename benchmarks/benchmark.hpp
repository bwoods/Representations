#pragma once

#define NDEBUG 1
#include "nonius/nonius.h++"

#include <algorithm>
#include <string>


namespace benchmarking {

template <typename T>
auto label() {

#ifdef _MSC_VER
	std::string string = __FUNCSIG__;
	auto begin = ++std::find(string.begin(), string.end(), '<');
	auto space = std::find(begin, string.end(), ' ');
	if (space != string.end())
		begin = ++space;

	auto end = std::find(begin, string.end(), '<');
	if (end == string.end())
		end = std::find(begin, string.end(), '>');

	return std::string(begin, end);
#else
	std::string string = __PRETTY_FUNCTION__;
	std::string prefix = "T = ";
	auto pos = string.find(prefix);
	pos += prefix.size();

	auto begin = std::next(string.begin(), pos);
	auto end = std::find(begin, string.end(), '<');
	if (end == string.end())
		end = std::find(begin, string.end(), ']');

	auto remove = [ ] (auto from, std::string const& substring)
	{
		size_t pos = from.find(substring);
		if (pos != std::string::npos)
			from.erase(pos, substring.length());
		return from;
	};

	string = std::string(begin, end);
	string = remove(string, "::__1");
	string = remove(string, "::__cxx11");

	return string;
#endif

}

static char const * html_template = R"(
<html>
<head>
</head>
<body>
{% for chart in charts %}<embed src='{$chart}' width='100%' height='440'>
{% endfor %}
</body>
</html>
)";


template <class Reporter, typename... Types>
struct benchmark {
	template <class Benchmark>
	static void report(std::string files, std::initializer_list<size_t> counts, std::string title, Benchmark&& benchmark) {
		cpptempl::data_list charts;

		nonius::configuration cfg;
		cfg.samples = 20;
		cfg.verbose = true;

		for (auto count : counts) {
			cpptempl::data_map map;
			map["test"] = cpptempl::make_data(title);
			map["suffix"] = cpptempl::make_data(std::to_string(count));

			cfg.output_file = cpptempl::parse(files, map);
			cfg.title = title + " — " + std::to_string(count);

			nonius::benchmark benchmarks[] = {
				(nonius::benchmark(label<Types>(), [=] (nonius::chronometer meter) {
					return benchmark(Types{}, count, meter);
				}))...
			};

			nonius::go(cfg, std::begin(benchmarks), std::end(benchmarks), Reporter{});
			charts.emplace_back(cfg.output_file);
		}

		cpptempl::data_map map;
		map["test"] = cpptempl::make_data(title);
		map["charts"] = cpptempl::make_data(charts);
		map["suffix"] = cpptempl::make_data("all");

		std::ofstream stream(cpptempl::parse(files, map), std::ios::binary);
		cpptempl::parse(stream, html_template, map);
		stream << std::flush;
	}

	template <class Benchmark, typename... Benchmarks>
	static auto suite(std::string files, std::initializer_list<size_t> counts, std::string title, Benchmark&& benchmark, Benchmarks&&... benchmarks) {
		report(files, counts, title, benchmark);
		suite(files, counts, benchmarks...);
	}

	static auto suite(std::string files, std::initializer_list<size_t> counts) {
		return;
	}
};


template <class Container>
auto __attribute__((noinline)) fill_n(Container& container, size_t n) {
	for (size_t i = 0; i < n; ++i)
		container.emplace_back(i);
}


}

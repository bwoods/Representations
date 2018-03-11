# Vector Representations

A collection of dynamic array data structures and benchmarks over them. A [snapshot](benchmarks/results.pdf) of the results can be found in the benchmarks folder.

Note that each implementation only implements a subset of [std::vector][]’s interface.

[std::vector]: http://en.cppreference.com/w/cpp/container/vector

## Benchmarking

Running the tests is as simple as

```sh
cmake -Bbuild -H.
cmake --build build --target vector_tests
cmake --build build --target benchmarks
cd build && ctest --output-on-failure
```

If the unit tests all pass, the report will be in [build/benchmarks.html](build/benchmarks.html).




#include "benchmark_common.hpp"
#include <chrono>

#define BM_SETUP \
    const index64 size = state.range(0); \
    std::string strToAdd = gen_random(size); \
    const char * str = strToAdd.c_str();

void SetBenchmarkParams(benchmark::internal::Benchmark * benchmark) {
    benchmark
        ->MinWarmUpTime(std::chrono::nanoseconds(std::chrono::seconds(5)).count())
        ->RangeMultiplier(2)->Range(1<<10, 1<<18)
        ->Complexity(benchmark::BigO::oAuto)
    ;
}

void BM_SmartBuffer_index(benchmark::State& state) {
    BM_SETUP;

    m::smart::Buffer hi(1);
    hi.Append(str, STRLEN(str));

    for (const auto _ : state) {
        for (m::smart::buffer::char_t c : hi) {
            benchmark::DoNotOptimize(c);
        }
    }
}
BENCHMARK(BM_SmartBuffer_index)->Apply(SetBenchmarkParams);

void BM_SmartBuffer_it(benchmark::State& state) {
    BM_SETUP;

    m::smart::Buffer hi(1);
    hi.Append(str, STRLEN(str));

    for (const auto _ : state) {
        for (m::smart::buffer::char_t c : hi) {
            benchmark::DoNotOptimize(c);
        }
    }
}
BENCHMARK(BM_SmartBuffer_it)->Apply(SetBenchmarkParams);

#if 1
void BM_Minibuffer_index(benchmark::State& state) {
    BM_SETUP;

    m::smart::buffer::SinglePtr hi = m::smart::buffer::SinglePtr::NewReadonly(str, STRLEN(str));

    // const could lead to unwanted optimization - benchmark::DoNotOptimize<T>(const T&) is deprecated
    m::smart::Buffer::char_t * s = const_cast<m::smart::Buffer::char_t *>(hi.Get());

    for (const auto _ : state) {
        for (m::smart::buffer::size_t i = 0; i < hi.Length(); i++) {
            benchmark::DoNotOptimize(s[i]);
        }
    }
}
BENCHMARK(BM_Minibuffer_index)->Apply(SetBenchmarkParams);
#endif

void BM_StdStringOwnAlloc_index(benchmark::State& state) {
    using string = std::basic_string<char, std::char_traits<char>, OwnAllocator<char> >;

    BM_SETUP;

    string hi(str);

    for (const auto _ : state) {
        for (int i = 0; i < hi.size(); i++) {
            benchmark::DoNotOptimize(hi[i]);
        }
    }
}
BENCHMARK(BM_StdStringOwnAlloc_index)->Apply(SetBenchmarkParams);

void BM_StdStringOwnAlloc_it(benchmark::State& state) {
    using string = std::basic_string<char, std::char_traits<char>, OwnAllocator<char> >;

    BM_SETUP;

    string hi(str);

    for (const auto _ : state) {
        for (char c : hi) {
            benchmark::DoNotOptimize(c);
        }
    }
}
BENCHMARK(BM_StdStringOwnAlloc_it)->Apply(SetBenchmarkParams);

void BM_StdString_index(benchmark::State& state) {
    BM_SETUP;

    std::string hi(str);

    for (const auto _ : state) {
        for (int i = 0; i < hi.size(); i++) {
            benchmark::DoNotOptimize(hi[i]);
        }
    }
}
BENCHMARK(BM_StdString_index)->Apply(SetBenchmarkParams);

void BM_StdString_it(benchmark::State& state) {
    BM_SETUP;

    std::string hi(str);

    for (const auto _ : state) {
        for (char c : hi) {
            benchmark::DoNotOptimize(c);
        }
    }
}
BENCHMARK(BM_StdString_it)->Apply(SetBenchmarkParams);

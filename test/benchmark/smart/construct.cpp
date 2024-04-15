#include "benchmark_common.hpp"

void SetBenchmarkParams(benchmark::internal::Benchmark * benchmark) {
    benchmark
        ->MinWarmUpTime(std::chrono::seconds(5).count())
        ->Complexity(benchmark::BigO::oAuto)
    ;
}

#if 0
void BM_SmartString(benchmark::State& state) {
    for (const auto _ : state) {
        smart::string::String hi;
        benchmark::DoNotOptimize(hi);
    }
}
BENCHMARK(BM_SmartString)->Apply(SetBenchmarkParams);
#endif

void BM_SmartBuffer(benchmark::State& state) {
    for (const auto _ : state) {
        m::smart::Buffer hi(10);
        benchmark::DoNotOptimize(hi);
    }
}
BENCHMARK(BM_SmartBuffer)->Apply(SetBenchmarkParams);

#if 1
void BM_Minibuffer(benchmark::State& state) {
    for (const auto _ : state) {
        m::smart::buffer::SinglePtr hi = m::smart::buffer::SinglePtr::New(0);
        benchmark::DoNotOptimize(hi);
    }
}
BENCHMARK(BM_Minibuffer)->Apply(SetBenchmarkParams);
#endif

void BM_StdStringOwnAlloc(benchmark::State& state) {
    using string = std::basic_string<char, std::char_traits<char>, OwnAllocator<char> >;

    for (const auto _ : state) {
        string hi;
        benchmark::DoNotOptimize(hi);
    }
}
BENCHMARK(BM_StdStringOwnAlloc)->Apply(SetBenchmarkParams);

void BM_StdString(benchmark::State& state) {
    for (const auto _ : state) {
        std::string hi;
        benchmark::DoNotOptimize(hi);
    }
}
BENCHMARK(BM_StdString)->Apply(SetBenchmarkParams);

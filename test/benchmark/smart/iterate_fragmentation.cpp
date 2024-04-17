#include "benchmark_common.hpp"
#include <chrono>

void SetBenchmarkParams(benchmark::internal::Benchmark * benchmark) {
    benchmark
        ->MinWarmUpTime(std::chrono::nanoseconds(std::chrono::seconds(5)).count())
        ->RangeMultiplier(2)->Range(1, 1<<10)
        ->Complexity(benchmark::BigO::oAuto)
    ;
}

const std::string str = gen_random(4096);
const m::smart::buffer::SinglePtr parent = m::smart::buffer::SinglePtr::NewReadonly(str.c_str(), str.size());

m::smart::Buffer SplitParent(const m::smart::buffer::SinglePtr& p, m::smart::buffer::size_t parts) {
    const m::smart::buffer::size_t childSize = p.Length() / parts;

    m::smart::Buffer res(parts);
    for (m::smart::buffer::size_t i = 0; i < parts; i++) {
        res.Add(p.Child(i * childSize, childSize));
    }

    return res;
}

void BM_SmartBuffer_index(benchmark::State& state) {
    const index64 size = state.range(0);
    const m::smart::Buffer hi = SplitParent(parent, size);

    for (const auto _ : state) {
        for (m::smart::buffer::size_t i = 0; i < hi.Length(); i++) {
            benchmark::DoNotOptimize(hi.FindIndex(i).Get());
        }
    }
}
BENCHMARK(BM_SmartBuffer_index)->Apply(SetBenchmarkParams);

void BM_SmartBuffer_it(benchmark::State& state) {
    const index64 size = state.range(0);
    const m::smart::Buffer hi = SplitParent(parent, size);

    for (const auto _ : state) {
        for (m::smart::buffer::char_t c : hi) {
            benchmark::DoNotOptimize(c);
        }
    }
}
BENCHMARK(BM_SmartBuffer_it)->Apply(SetBenchmarkParams);

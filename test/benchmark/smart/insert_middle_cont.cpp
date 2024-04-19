#include "benchmark_common.hpp"

const index64 size = 4000;
const std::string strToAdd = gen_random(size);
const char * str = strToAdd.c_str();
const index64 middle = size / 2;


const m::smart::buffer::SinglePtr bufToAdd = m::smart::buffer::SinglePtr::NewStatic(str, STRLEN(str));

void SetBenchmarkParams(benchmark::internal::Benchmark * benchmark) {
    benchmark
        ->MinWarmUpTime(std::chrono::seconds(5).count())
        //->RangeMultiplier(2)->Range(1<<10, 1<<18)
        ->RangeMultiplier(3)->Range(1, 1<<10)
        ->Complexity(benchmark::BigO::oAuto)
    ;
}

void BM_SmartBuffer(benchmark::State& state) {
    const int64_t numIterations = state.range(0);

    for (const auto _ : state) {
        MM_START;

        m::smart::Buffer hi(10);
        hi.Add(bufToAdd);

        for (int64_t i = 0; i < numIterations; i++) {
            hi.Insert(middle, str, STRLEN(str));
        }

        MM_STOP;
    }

    state.SetComplexityN(numIterations);
}
BENCHMARK(BM_SmartBuffer)->Apply(SetBenchmarkParams);

#if 1
void BM_Minibuffer(benchmark::State& state) {
    const int64_t numIterations = state.range(0);

    for (const auto _ : state) {
        MM_START;
        m::smart::buffer::SinglePtr hi = m::smart::buffer::SinglePtr::New(bufToAdd.Length());
        hi.Append(bufToAdd.Get(), bufToAdd.Length());

        for (int64_t i = 0; i < numIterations; i++) {
            hi.Insert(middle, (m::smart::buffer::char_t *) str, STRLEN(str));
        }

        MM_STOP;
    }

    state.SetComplexityN(numIterations);
}
BENCHMARK(BM_Minibuffer)->Apply(SetBenchmarkParams);
#endif

void BM_StdStringOwnAlloc(benchmark::State& state) {
    using string = std::basic_string<char, std::char_traits<char>, OwnAllocator<char> >;

    const int64_t numIterations = state.range(0);

    for (const auto _ : state) {
        MM_START;

        string hi(str);

        for (int64_t i = 0; i < numIterations; i++) {
            hi.insert(middle, str);
        }

        MM_STOP;
    }

    state.SetComplexityN(numIterations);
}
BENCHMARK(BM_StdStringOwnAlloc)->Apply(SetBenchmarkParams);

#ifndef MEMORY_TEST
void BM_StdString(benchmark::State& state) {
    const int64_t numIterations = state.range(0);

    for (const auto _ : state) {
        std::string hi(str);

        for (int64_t i = 0; i < numIterations; i++) {
            hi.insert(middle, str);
        }
    }

    state.SetComplexityN(numIterations);
}
BENCHMARK(BM_StdString)->Apply(SetBenchmarkParams);
#endif

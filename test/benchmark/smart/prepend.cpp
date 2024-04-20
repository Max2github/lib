#include "benchmark_common.hpp"

// TODO: may be inefficient, as the PauseTiming and ResumeTiming take time too...
// may test without pausing and subtract the time for initialization (make an own test for that) 
#define BM_INIT(code) \
    state.PauseTiming(); \
    code; \
    state.ResumeTiming();

#define BM_SETUP \
    const index64 size = state.range(0); \
    std::string strToAdd = gen_random(size); \
    const char * str = strToAdd.c_str();

#define BM_SET_COMPLEXITY_N state.SetComplexityN(size)

void SetBenchmarkParams(benchmark::internal::Benchmark * benchmark) {
    benchmark
        ->MinWarmUpTime(std::chrono::seconds(5).count())
        ->RangeMultiplier(2)->Range(1<<10, 1<<18)
        //->RangeMultiplier(2)->Range(1, 1<<10)
        ->Complexity(benchmark::BigO::oAuto)
    ;
}

void BM_SmartBuffer(benchmark::State& state) {
    BM_SETUP;

    for (const auto _ : state) {
        MM_START;

        BM_INIT(
            m::smart::Buffer hi(10);
            hi.Append(str, STRLEN(str));
        );

        hi.Prepend(str, STRLEN(str));
        MM_STOP;
    }

    BM_SET_COMPLEXITY_N;
}
BENCHMARK(BM_SmartBuffer)->Apply(SetBenchmarkParams);

#if 1
void BM_Minibuffer(benchmark::State& state) {
    BM_SETUP;

    for (const auto _ : state) {
        MM_START;
        BM_INIT(
            m::smart::buffer::SinglePtr hi = m::smart::buffer::SinglePtr::New(STRLEN(str));
            hi.Append(str, STRLEN(str));
        );

        hi.Insert(0, (m::smart::buffer::char_t *) str, STRLEN(str));
        MM_STOP;
    }

    BM_SET_COMPLEXITY_N;
}
BENCHMARK(BM_Minibuffer)->Apply(SetBenchmarkParams);
#endif

void BM_StdStringOwnAlloc(benchmark::State& state) {
    using string = std::basic_string<char, std::char_traits<char>, OwnAllocator<char> >;

    BM_SETUP;

    for (const auto _ : state) {
        MM_START;
        BM_INIT(string hi(str));

        hi.insert(0, str);
        MM_STOP;
    }

    BM_SET_COMPLEXITY_N;
}
BENCHMARK(BM_StdStringOwnAlloc)->Apply(SetBenchmarkParams);

#ifndef MEMORY_TEST
void BM_StdString(benchmark::State& state) {
    BM_SETUP;

    for (const auto _ : state) {
        MM_START;
        BM_INIT(std::string hi(str));

        hi.insert(0, str);
        MM_STOP;
    }

    BM_SET_COMPLEXITY_N;
}
BENCHMARK(BM_StdString)->Apply(SetBenchmarkParams);
#endif

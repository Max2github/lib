#include "benchmark_common.hpp"

const std::string strToAdd = gen_random(4000);
const char * str = strToAdd.c_str();

// ensure that we all use the same strlen function (like std::string)
#define STRLEN(s) std::char_traits<char>::length(s)

void SetBenchmarkParams(benchmark::internal::Benchmark * benchmark) {
    benchmark
        //->MinWarmUpTime(std::chrono::seconds(5).count())
        ->Iterations(1000)
        //->RangeMultiplier(2)->Range(1<<10, 1<<18)
        ->RangeMultiplier(2)->Range(1, 1<<10)
        ->Complexity(benchmark::BigO::oAuto)
    ;
}

#if 0
void BM_SmartString(benchmark::State& state) {
    const int64_t numIterations = state.range(0);

    for (const auto _ : state) {
        smart::string::String hi;

        for (int64_t i = 0; i < numIterations; i++) {
            hi.Append(str);
        }
    }

    state.SetComplexityN(numIterations);
}
BENCHMARK(BM_SmartString)->Apply(SetBenchmarkParams);
#endif

void BM_SmartBuffer(benchmark::State& state) {
    const int64_t numIterations = state.range(0);

    for (const auto _ : state) {
        m::smart::Buffer hi(10);
        //hi.Add(m::smart::buffer::SinglePtr(str, STRLEN(str)));

        for (int64_t i = 0; i < numIterations; i++) {
            hi.Add(m::smart::buffer::SinglePtr::NewReadonly(str, STRLEN(str)));
        }
    }

    state.SetComplexityN(numIterations);
}
BENCHMARK(BM_SmartBuffer)->Apply(SetBenchmarkParams);

#if 1
void BM_Minibuffer(benchmark::State& state) {
    const int64_t numIterations = state.range(0);

    for (const auto _ : state) {
        m::smart::Buffer::SinglePtr hi((m::smart::Buffer::size_t) 0);
        //m::smart::Buffer::SinglePtr hi(str, STRLEN(str));

        for (int64_t i = 0; i < numIterations; i++) {
            hi.Append((m::smart::buffer::char_t *) str, STRLEN(str));
        }
    }

    state.SetComplexityN(numIterations);
}
BENCHMARK(BM_Minibuffer)->Apply(SetBenchmarkParams);
#endif

#if 0
void BM_CString(benchmark::State& state) {
    for (const auto _ : state) {
        char * hi = (char *) malloc(STRLEN(str)+1);

        for (int64_t i = 0; i < numIterations; i++) {
            hi = (char *) realloc(hi, STRLEN(hi) + STRLEN(str) + 1);
            hi = strcat(hi, str);
        }

        free(hi);
    }
    state.SetComplexityN(numIterations);
}
BENCHMARK(BM_CString)->Apply(SetBenchmarkParams);
#endif

void BM_StdStringOwnAlloc(benchmark::State& state) {
    using string = std::basic_string<char, std::char_traits<char>, OwnAllocator<char> >;

    const int64_t numIterations = state.range(0);

    for (const auto _ : state) {
        string hi;
        //string hi(str);

        for (int64_t i = 0; i < numIterations; i++) {
            hi.append(str);
        }
    }

    state.SetComplexityN(numIterations);
}
BENCHMARK(BM_StdStringOwnAlloc)->Apply(SetBenchmarkParams);

#ifndef MEMORY_TEST
void BM_StdString(benchmark::State& state) {
    const int64_t numIterations = state.range(0);

    for (const auto _ : state) {
        std::string hi;
        //std::string hi(str);

        for (int64_t i = 0; i < numIterations; i++) {
            hi.append(str);
        }
    }

    state.SetComplexityN(numIterations);
}
BENCHMARK(BM_StdString)->Apply(SetBenchmarkParams);
#endif

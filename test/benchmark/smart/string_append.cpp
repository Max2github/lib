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

#if 0
void BM_SmartString(benchmark::State& state) {
    BM_SETUP;

    for (const auto _ : state) {
        BM_INIT(smart::string::String hi);

        hi.Append(str);
    }

    BM_SET_COMPLEXITY_N;
}
BENCHMARK(BM_SmartString)->Apply(SetBenchmarkParams);
#endif

void BM_SmartBuffer(benchmark::State& state) {
    BM_SETUP;

    for (const auto _ : state) {
        BM_INIT(m::smart::Buffer hi(10));

        hi.Add(m::smart::Buffer::SinglePtr::NewReadonly(str, STRLEN(str)));
    }

    BM_SET_COMPLEXITY_N;
}
BENCHMARK(BM_SmartBuffer)->Apply(SetBenchmarkParams);

#if 1
void BM_Minibuffer(benchmark::State& state) {
    BM_SETUP;

    for (const auto _ : state) {
        BM_INIT(m::smart::buffer::SinglePtr hi);
        hi.Append((m::smart::buffer::char_t *) str, STRLEN(str));
    }

    BM_SET_COMPLEXITY_N;
}
BENCHMARK(BM_Minibuffer)->Apply(SetBenchmarkParams);
#endif

#if 1
void BM_CString(benchmark::State& state) {
    BM_SETUP;

    for (const auto _ : state) {
        BM_INIT(char * hi = NULL);

        hi = (char *) BM_MALLOC(size+1);
        *hi = '\0';
        strcat(hi, str);
    }

    BM_SET_COMPLEXITY_N;
}
BENCHMARK(BM_CString)->Apply(SetBenchmarkParams);
#endif

void BM_StdStringOwnAlloc(benchmark::State& state) {
    using string = std::basic_string<char, std::char_traits<char>, OwnAllocator<char> >;

    BM_SETUP;

    for (const auto _ : state) {
        BM_INIT(string hi);

        hi.append(str);
    }

    BM_SET_COMPLEXITY_N;
}
BENCHMARK(BM_StdStringOwnAlloc)->Apply(SetBenchmarkParams);

void BM_StdString(benchmark::State& state) {
    BM_SETUP;

    for (const auto _ : state) {
        BM_INIT(std::string hi);

        hi.append(str);
    }

    BM_SET_COMPLEXITY_N;
}
BENCHMARK(BM_StdString)->Apply(SetBenchmarkParams);

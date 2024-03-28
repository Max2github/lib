#include <iostream>

#include <numeric>
#include <string>

#include "own_allocator.hpp"
#include "string_include.hpp"

#include <benchmark/benchmark.h>
#include <string>
#include <cstring>
#include <ctime>
#include <iostream>
#include <unistd.h>
#include <chrono>

std::string gen_random(const int len) noexcept {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    
    return tmp_s;
}

// So that std::string uses the same allocator as m::smart::String
template <class T>
struct OwnAllocator {
    typedef T value_type;
    OwnAllocator() noexcept {}

    //typedef T * const_pointer;

    // A converting copy constructor:
    template<class U> OwnAllocator(const OwnAllocator<U>&) noexcept {}

    template<class U> bool operator==(const OwnAllocator<U>&) const noexcept { return true; }
    template<class U> bool operator!=(const OwnAllocator<U>&) const noexcept { return false; }

    T * allocate(const size_t n) const {
        if (n == 0) { return nullptr; }
        if (n > static_cast<size_t>(-1) / sizeof(T)) {
            throw std::bad_array_new_length();
        }
        void * const pv = SMARTSTRING_HPP_MALLOC(n * sizeof(T));
        if (!pv) { throw std::bad_alloc(); }
        return static_cast<T*>(pv);
    }
    void deallocate(T* const p, size_t) const noexcept {
        SMARTSTRING_HPP_FREE(p);
    }
};

const std::string strToAdd = gen_random(4000);
const char * str = strToAdd.c_str();

//const auto rng = std::vector<int>();

void BM_SmartString(benchmark::State& state) {
    smart::string::String hi(str);

    for (const auto _ : state) {
        hi.Append(str);
    }
    state.SetComplexityN(state.iterations());
    //state.SetComplexityN(state.range(0));
}

void BM_SmartBuffer(benchmark::State& state) {
    m::smart::Buffer hi(10);
    hi.Add(m::smart::buffer::SinglePtr(str, strlen(str)));

    for (const auto _ : state) {
        hi.Add(m::smart::buffer::SinglePtr(str, strlen(str)));
    }
    state.SetComplexityN(state.iterations());
    //state.SetComplexityN(state.range(0));
}

#if 1
void BM_Minibuffer(benchmark::State& state) {
    m::smart::buffer::SinglePtr hi((m::smart::buffer::char_t *) str, strlen(str));

    for (const auto _ : state) {
        hi.Append((m::smart::buffer::char_t *) str, strlen(str));
    }

    state.SetComplexityN(state.iterations());
    //state.SetComplexityN(state.range(0));
}
#endif

#if 0
void BM_CString(benchmark::State& state) {
    char * hi = (char *) malloc(strlen(str)+1);
    strcpy(hi, str);
    for (const auto _ : state) {
        hi = (char *) realloc(hi, strlen(hi) + strlen(str) + 1);
        hi = strcat(hi, str);
    }
    free(hi);
    state.SetComplexityN(state.iterations());
    //state.SetComplexityN(state.range(0));
}
#endif

void BM_StdStringOwnAlloc(benchmark::State& state) {
    using string = std::basic_string<char, std::char_traits<char>, OwnAllocator<char> >;
    string hi(str);
    for (const auto _ : state) {
        hi.append(str);
    }
    state.SetComplexityN(state.iterations());
    //state.SetComplexityN(state.range(0));
}

void BM_StdString(benchmark::State& state) {
    std::string hi(str);
    for (const benchmark::State::StateIterator::Value it : state) {
        hi.append(str);
    }
    state.SetComplexityN(state.iterations());
    //state.SetComplexityN(state.range(0));
}

void SetBenchmarkParams(benchmark::internal::Benchmark * benchmark) {
    benchmark
        ->MinWarmUpTime(std::chrono::seconds(5).count())
        ->Iterations(100)
        //->RangeMultiplier(2)->Range(1<<10, 1<<18)
        ->RangeMultiplier(2)->Range(1, 1<<10)
        ->Complexity(benchmark::BigO::oAuto)
    ;
}

BENCHMARK(BM_SmartString)->Apply(SetBenchmarkParams);
BENCHMARK(BM_SmartBuffer)->Apply(SetBenchmarkParams);
//BENCHMARK(BM_CString)->Apply(SetBenchmarkParams);
BENCHMARK(BM_Minibuffer)->Apply(SetBenchmarkParams);
BENCHMARK(BM_StdString)->Apply(SetBenchmarkParams);
BENCHMARK(BM_StdStringOwnAlloc)->Apply(SetBenchmarkParams);

BENCHMARK_MAIN();

#ifndef BENCHMARK_COMMON_HPP
#define BENCHMARK_COMMON_HPP

#include <benchmark/benchmark.h>
#include <chrono>
#include <functional>

#include "own_allocator.hpp"
#include "MemoryCounter.hpp"
#include "string_include.hpp"

#include <string>

#if LANG_CPP_STD >= 2011
    #define NOEXCEPT noexcept
    #define OVERRIDE override
#else
    #define NOEXCEPT
    #define OVERRIDE
#endif

std::string gen_random(const int len) NOEXCEPT {
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
    OwnAllocator() NOEXCEPT {}

    //typedef T * const_pointer;

    // A converting copy constructor:
    template<class U> OwnAllocator(const OwnAllocator<U>&) NOEXCEPT {}

    template<class U> bool operator==(const OwnAllocator<U>&) const NOEXCEPT { return true; }
    template<class U> bool operator!=(const OwnAllocator<U>&) const NOEXCEPT { return false; }

    T * allocate(const size_t n) const {
        if (n == 0) { return nullptr; }
        if (n > static_cast<size_t>(-1) / sizeof(T)) {
            throw std::bad_array_new_length();
        }
        void * const pv = SMARTSTRING_HPP_MALLOC(n * sizeof(T));
        if (!pv) { throw std::bad_alloc(); }
        return static_cast<T*>(pv);
    }
    void deallocate(T* const p, size_t) const NOEXCEPT {
        SMARTSTRING_HPP_FREE(p);
    }
};

class CustomTimer {
public:
    static CustomTimer Start() { return CustomTimer(std::chrono::high_resolution_clock::now()); }
    std::chrono::duration<double> Stop() {
        const std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::duration<double> >(end - m_start);
    }

private:
    CustomTimer(const std::chrono::high_resolution_clock::time_point& start) : m_start(start) {}

    #if LANG_CPP_STD >= 2011
    CustomTimer(std::chrono::high_resolution_clock::time_point&& start) : m_start(start) {}
    #endif

private:
    std::chrono::high_resolution_clock::time_point m_start;
};

#ifdef MEMORY_TEST
int main(int argc, char** argv) {
    char arg0_default[] = "benchmark";
    char* args_default = arg0_default;
    if (!argv) { argc = 1; argv = &args_default; }
    ::benchmark::RegisterMemoryManager(&MemoryCounter::mm);
    ::benchmark::Initialize(&argc, argv);
    if (::benchmark::ReportUnrecognizedArguments(argc, argv)) return 1;
    ::benchmark::RunSpecifiedBenchmarks();
    ::benchmark::Shutdown();
    return 0;
}
#else
BENCHMARK_MAIN();
#endif

#endif // BENCHMARK_COMMON_HPP

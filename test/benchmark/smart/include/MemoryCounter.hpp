#ifndef MEMORY_COUNTER_HPP
#define MEMORY_COUNTER_HPP

#include "../../../../templates/def.h"
#include <benchmark/benchmark.h>

#if OS_WINDOWS && defined(MEMORY_TEST)
    const int64_t benchmark::MemoryManager::TombstoneValue =
    std::numeric_limits<int64_t>::max();
#endif

class MemoryCounter: public benchmark::MemoryManager {
public:
    typedef void*(*alloc_f)(size_t);
    typedef void*(*realloc_f)(void*, size_t);
    typedef void(*free_f)(void*);

    MemoryCounter() : m_result(), m_started(false), m_map() {

    }

    void Start() BENCHMARK_OVERRIDE {
        // reset to default
        m_result = benchmark::MemoryManager::Result();
        m_result.num_allocs = 0;
        m_result.max_bytes_used = 0;
        m_result.total_allocated_bytes = 0;
        m_result.net_heap_growth = 0;
        //m_total_deallocated_bytes = 0;
        m_started = true;
    }

    void Stop() {
        m_map.clear();
        m_started = false;
    }

    void Stop(Result& result) BENCHMARK_OVERRIDE {
        result = m_result;
        this->Stop();
    }

    void * Alloc(int64_t size, alloc_f a) {
        void * res = a(size);
        if (m_started) { this->StatAlloc(res, size); }
        return res;
    }

    void * Realloc(void * oldptr, int64_t size, realloc_f realloc) {
        void * res = realloc(oldptr, size);
        if (m_started) { this->StatRealloc(res, oldptr, size); }
        return res;
    }

    void Free(void * ptr, free_f free) {
        free(ptr);
        if (m_started && ptr != NULL) { this->StatFree(ptr); }
    }

private:
    void StatAlloc(void * ptr, int64_t size) {
        m_result.num_allocs += 1;
        m_result.total_allocated_bytes += size;
        m_result.net_heap_growth += size;
        if (m_result.net_heap_growth > m_result.max_bytes_used) {
            m_result.max_bytes_used = m_result.net_heap_growth;
        }
        m_map.insert({ ptr, size });
    }

    void StatRealloc(void * newptr, void * oldptr, int64 newsize) {
        if (newptr != oldptr) {
            this->StatFree(oldptr);
            this->StatAlloc(newptr, newsize);
        } else {
            int64_t& sizeInTable = m_map.at(oldptr);
            const int64_t oldSize = sizeInTable;
            sizeInTable = newsize;
            const int64_t diff = newsize - oldSize;
            if (diff > 0) {
                m_result.total_allocated_bytes += diff;
            }
            m_result.num_allocs += 1;
            m_result.net_heap_growth += diff;
            if (m_result.net_heap_growth > m_result.max_bytes_used) {
                m_result.max_bytes_used = m_result.net_heap_growth;
            }
        }
    }

    void StatFree(void * ptr) {
        const int64_t size = m_map.at(ptr);
        m_map.erase(ptr);
        //m_total_deallocated_bytes += size;
        m_result.net_heap_growth -= size;
    }

public:
    static MemoryCounter mm;

private:
    benchmark::MemoryManager::Result m_result;

    bool m_started;

    std::map<void *, int64_t> m_map;
};

#endif // MEMORY_COUNTER_HPP

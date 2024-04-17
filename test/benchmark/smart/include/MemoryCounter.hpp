#include "../../../../templates/def.h"
#include <benchmark/benchmark.h>

class MemoryCounter: public benchmark::MemoryManager {
public:
    typedef void*(*alloc_f)(size_t);
    typedef void*(*realloc_f)(void*, size_t);
    typedef void(*free_f)(void*);

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
    //int64_t m_total_deallocated_bytes;

    bool m_started = false;

    //alloc_f m_alloc;
    //realloc_f m_realloc;
    //free_f m_free;

    std::map<void *, int64_t> m_map;
};

#ifdef MEMORY_TEST
MemoryCounter MemoryCounter::mm;

#define SMARTSTRING_HPP_MALLOC(size) MemoryCounter::mm.Alloc(size, malloc);
#define SMARTSTRING_HPP_REALLOC(oldP, size, oldSize) MemoryCounter::mm.Realloc(oldP, size, realloc);
#define SMARTSTRING_HPP_FREE(p) MemoryCounter::mm.Free(p, free);

#define MM_STOP MemoryCounter::mm.Stop()
#else
#define MM_STOP
#endif

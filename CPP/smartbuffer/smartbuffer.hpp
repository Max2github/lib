#ifndef SMARTBUFFER_HPP
#define SMARTBUFFER_HPP

#include "../../templates/def.h"

#ifndef NO_STD_LIB
    #include<stdlib.h>

    #ifndef SMARTBUFFER_HPP_MALLOC
    #define SMARTBUFFER_HPP_MALLOC(size) malloc(size)
    #endif

    #ifndef SMARTBUFFER_HPP_REALLOC
    #define SMARTBUFFER_HPP_REALLOC(oldP, size, oldSize) realloc(oldP, size)
    #endif

    #ifndef SMARTBUFFER_HPP_FREE
    #define SMARTBUFFER_HPP_FREE(p) free(p)
    #endif
#endif

#ifndef SMARTBUFFER_CUSTOM
#define SMARTBUFFER_CUSTOM
#define SMARTBUFFER_CHAR index8
#define SMARTBUFFER_LEN_T index64
#define SMARTBUFFER_BOOL_T bool
#define SMARTBUFFER_TRUE true
#define SMARTBUFFER_FALSE false
#endif

#ifndef SMARTBUFFER_H_MALLOC
#define SMARTBUFFER_H_MALLOC SMARTBUFFER_HPP_MALLOC
#endif

#ifndef SMARTBUFFER_H_REALLOC
#define SMARTBUFFER_H_REALLOC SMARTBUFFER_HPP_REALLOC
#endif

#ifndef SMARTBUFFER_H_FREE
#define SMARTBUFFER_H_FREE SMARTBUFFER_HPP_FREE
#endif

#include "../../smartbuffer.h"

#include <utility>

namespace m {
    namespace smart {

        class Buffer;

        namespace buffer {
            typedef SMARTBUFFER_LEN_T size_t;
            typedef size_t index_t;
            typedef SMARTBUFFER_CHAR char_t;
            typedef sBuffer_readHandler reader_t;

            class SinglePtr {
            public:
                // null
                SinglePtr(nullptr_t);

                // from internal
                SinglePtr(const sBuffer_single_ptr&); // copy

                #if LANG_CPP_STD >= 2011
                SinglePtr(sBuffer_single_ptr&&); // move
                #endif

                // from other
                SinglePtr(const SinglePtr&); // copy

                #if LANG_CPP_STD >= 2011
                SinglePtr(SinglePtr&&); // move
                #endif

                ~SinglePtr();

                // static create methods
                static SinglePtr New(size_t size);
                static SinglePtr NewOnce(size_t size);
                static SinglePtr NewReadonly(const char_t * data, size_t len);
                static SinglePtr NewUnmanaged(const char_t * data, size_t len, size_t size);
                static SinglePtr NewStatic(const char_t * data, size_t len);

                bool IsNull() const;
                bool IsReadOnly() const;
                bool IsStatic() const; // if static it will also be read only
                bool IsChild() const;

                size_t Length() const;
                size_t Capacity() const;
                const char_t * Get() const;

                /**
                 * @brief create a child buffer
                 * 
                 * @param begin the index where the child buffer starts
                 * @param size the size of the child buffer
                 * @return SinglePtr 
                 */
                SinglePtr Child(index_t begin, size_t size) const;

                /**
                 * @brief return m_buffer and set this to NULL
                 * The user will have to handle the memory management.
                 * 
                 * @return char_t * 
                 */
                char_t * Release();

                size_t Write(index_t startIndex, const char_t * data, size_t length);
                size_t Append(const char_t * data, size_t length);
                size_t Insert(index_t start, const char_t * data, size_t Length);

                size_t Clear();
                void Delete();

                char_t operator[](index_t index);

            private:
                void SetNull(); // this does not free the data!!!
                const sBuffer_single_ptr& GetIntern() const;

                index_t UsageCountIncrease();
                index_t UsageCountDecrease();
            private:
                sBuffer_single_ptr m_buffer;

                friend class m::smart::Buffer;
            };

            class Iterator {
                public:
                    static Iterator Start(const Buffer& buffer);
                    static Iterator AtIndex(const Buffer& buffer, index_t index);
                    static Iterator End();

                    Iterator& Next();
                    Iterator& Advance(size_t len);

                    Iterator& GoToStart();
                    Iterator& GoToIndex(index_t index);
                    Iterator& GoToLast();
                    Iterator& SetEnd();

                    char_t Get() const;

                    bool IsLastElement() const;

                    inline Iterator& operator++() { return this->Next(); }
                    inline Iterator& operator++(int) { return this->Next(); } // TODO: this is not right - would in fact need to copy the iterator and return a copy
                    inline char_t operator*() const { return this->Get(); }

                    friend bool operator==(const Iterator& it1, const Iterator& it2);
                    inline friend bool operator!=(const Iterator& it1, const Iterator& it2) {
                        return !(it1 == it2);
                    }

                private:
                    explicit Iterator(const sBuffer * buffer);
                    Iterator(const sBuffer * buffer, const sBuffer_index_descr& indexDescr);
                    Iterator& NextSingle();
                    size_t AdvanceSingle(size_t len);
                    bool IsLastInSingle() const;
                    bool IsLastSingle() const;

                    static sBuffer_index_descr GetStartDescr(const sBuffer *);
                    static sBuffer_index_descr GetLastDescr(const sBuffer *);
                    static sBuffer_index_descr GetNullDescr();

                private:
                    const sBuffer * m_buffer;
                    sBuffer_index_descr m_current;
                    size_t m_curlen;

                friend class m::smart::Buffer;
            };

        } // namespace buffer

        class Buffer {
            public:
                typedef buffer::char_t char_t;
                typedef buffer::index_t index_t;
                typedef buffer::size_t size_t;
                typedef buffer::reader_t reader_t;
                typedef buffer::SinglePtr SinglePtr;
                typedef buffer::Iterator Iterator;
            public:
                Buffer(size_t size);

                Buffer(const sBuffer& buf); // internal

                Buffer(const Buffer&); // copy
                
                #if LANG_CPP_STD >= 2011
                Buffer(Buffer&&); // move
                #endif

                ~Buffer();

                void Delete();

                index_t Count() const; // number of SmartBufferSinglePtr
                size_t Length() const; // number of chars (in total)
                SinglePtr Get(index_t index) const;

                void Add(const SinglePtr&);
                void InsertSingle(index_t index, const SinglePtr&);

                #if LANG_CPP_STD >= 2011
                void Add(SinglePtr&&);
                void InsertSingle(index_t index, SinglePtr&&);
                #endif

                size_t Insert(index_t index, const char_t * data, size_t len);
                size_t Append(const char_t * data, size_t len);
                size_t Prepend(const char_t * data, size_t len);

                void Clear();

                size_t Read(const Iterator& pos, size_t length, reader_t readerFunction, void * userData = nullptr) const;
                size_t ReadTo(char_t * dest, const Iterator& pos, size_t len) const;
                size_t ReadTo(SinglePtr& dest, const Iterator& pos) const;
    #if 0
                template<typename... args_t>
                void ForEach(void (*func)(args_t...), args_t... args) {
                    SMARTBUFFER_FOREACH_V(i, cur, m_buffer,
                        func(std::forward<args_t>(args)...);
                    )
                }
    #endif
    #if LANG_CPP_STD >= 2011
                template<typename... args_t>
                using ForEachFunc_t = void (*)(const SinglePtr&, args_t...);

                template<typename... args_t>
                inline void ForEach(ForEachFunc_t<args_t...> func, args_t... args) const {
                    for (index_t i = 0; i < this->Count(); i++) {
                        func(this->Get(i), args...);
                    }
                }

                template<typename Func, typename... args_t>
                inline void ForEach(Func&& func, args_t... args) const {
                    for (index_t i = 0; i < this->Count(); i++) {
                        func(this->Get(i), args...);
                    }
                }
    #endif

                Iterator begin() const;
                Iterator end() const;

                Iterator FindIndex(index_t index) const;

    //#if 0
                Buffer& operator=(const Buffer&);

                #if LANG_CPP_STD >= 2011
                Buffer& operator=(Buffer&&);
                #endif

                Buffer operator+(const SinglePtr&) const;

                #if LANG_CPP_STD >= 2011
                Buffer operator+(SinglePtr&&) const;
                #endif
    //#endif
                inline Buffer& operator+=(const SinglePtr& other) { this->Add(other); return *this; }

                #if LANG_CPP_STD >= 2011
                inline Buffer& operator+=(SinglePtr&& other) { this->Add(other); return *this; }
                #endif

                //inline SmartBuffer& operator+=(const SmartBuffer& other) { this->ForEach<SmartBuffer *, const SmartBufferSinglePtr&>::run(this, &SmartBuffer::Add, this, other); }
                inline Buffer& operator+=(const Buffer& other) {
                    SMARTBUFFER_FOREACH_V(i, cur, other.m_buffer,
                        this->Add(cur);
                    )
                    return *this;
                }

                #if LANG_CPP_STD >= 2011
                inline Buffer& operator+=(Buffer&& other) {
                    SMARTBUFFER_FOREACH_V(i, cur, other.m_buffer,
                        this->Add(cur);
                    )
                    return *this;
                }
                #endif
            private:
                void UsageIncreaseAll();
                void UsageDecreaseAll();
            private:
                sBuffer m_buffer;

                friend class m::smart::buffer::Iterator;
        };

    } // namespace smart
} // namespace m

#endif // !SMARTBUFFER_HPP

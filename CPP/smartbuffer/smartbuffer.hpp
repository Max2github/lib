#ifndef SMARTBUFFER_HPP
#define SMARTBUFFER_HPP

#include "../../templates/def.h"

#ifndef NO_STD_LIB
    #include<stdlib.h>

    #ifndef SMARTBUFFER_HPP_MALLOC
    #define SMARTBUFFER_HPP_MALLOC(size) malloc(size)
    #endif

    #ifndef SMARTBUFFER_HPP_REALLOC
    #define SMARTBUFFER_HPP_REALLOC(oldP, size) realloc(oldP, size)
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
                SinglePtr(size_t size = 0);

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
            private:
                void SetNull(); // this does not free the data!!!
                const sBuffer_single_ptr& GetIntern() const;

                index_t UsageCountIncrease();
                index_t UsageCountDecrease();
            private:
                sBuffer_single_ptr m_buffer;

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

                #if LANG_CPP_STD >= 2011
                void Add(const SinglePtr&&);
                #endif

                //size_t Append(const char_t * data, );

                void Clear();

                size_t Read(reader_t readerFunction, size_t length, void * userData = nullptr) const;
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
    //#if 0
                Buffer& operator=(const Buffer&);

                #if LANG_CPP_STD >= 2011
                Buffer& operator=(const Buffer&&);
                #endif

                Buffer operator+(const SinglePtr&) const;

                #if LANG_CPP_STD >= 2011
                Buffer operator+(const SinglePtr&&) const;
                #endif
    //#endif
                inline Buffer& operator+=(const SinglePtr& other) { this->Add(other); return *this; }

                #if LANG_CPP_STD >= 2011
                inline Buffer& operator+=(const SinglePtr&& other) { this->Add(other); return *this; }
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
            };

    } // namespace smart
} // namespace m

#endif // !SMARTBUFFER_HPP

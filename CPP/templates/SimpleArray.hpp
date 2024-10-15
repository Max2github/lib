#ifndef SIMPLE_ARRAY_HPP
#define SIMPLE_ARRAY_HPP

#include <m/templates/simple_array.h>

namespace m {
    namespace templates {

        template<typename T>
        class SimpleArray {
            public:
                typedef SIMPLE_ARRAY(T) arr_t;
                typedef indexP size_t;

            public:
                SimpleArray() : m_arr((arr_t) SIMPLE_ARRAY_CREATE(T)) {}
                explicit SimpleArray(size_t size) : m_arr((arr_t) SIMPLE_ARRAY_CREATE_SIZE(T, size)) {}

                /**
                 * @brief Construct from an intern SIMPLE_ARRAY
                 * 
                 * The array will be copied.
                 * Type must be correct!!
                 * 
                 * @param other
                 */
                explicit SimpleArray(const arr_t& other) { SIMPLE_ARRAY_COPY(m_arr, other, T); }

                SimpleArray(const SimpleArray& other) { this->Set(other); }

                ~SimpleArray() { SIMPLE_ARRAY_FREE(m_arr); }

                // modify

                SimpleArray& Append(const T& item) { SIMPLE_ARRAY_APPEND(m_arr, item); return *this; }
                SimpleArray& AppendData(const T * data, size_t amount) { SIMPLE_ARRAY_APPEND_DATA(m_arr, data, amount); return *this; }
                SimpleArray& Set(const T * data, size_t amount) { SIMPLE_ARRAY_SET(m_arr, data, amount); return *this; }
                SimpleArray& Write(size_t index, const T * data, size_t amount) { SIMPLE_ARRAY_WRITE(m_arr, index, data, amount); return *this; }
                SimpleArray& Write(size_t index, const T& item) { this->Write(index, &item, 1); return *this; }
                SimpleArray& ShiftRight(size_t index, size_t amount) { SIMPLE_ARRAY_SHIFT_RIGHT(m_arr, index, amount); return *this; }

                T& Get(size_t index) { return InternGet(index); }

                // const

                const T& Get(size_t index) const { return InternGet(index); }
                SimpleArray<T> Copy() const { return this->Copy(*this); }

                // operators

                SimpleArray& operator=(const SimpleArray& other) { return this->Set(other); }
                T& operator[](size_t index) { return this->Get(index); }
                const T& operator[](size_t index) const { return this->Get(index); }

                // static methods

                static SimpleArray<T> Copy(const SimpleArray& other) {
                    arr_t newArr;
                    SIMPLE_ARRAY_COPY(newArr, other.m_arr, T);
                    return SimpleArray<T>::Wrap(newArr);
                }

                /**
                 * @brief Convert an intern SIMPLE_ARRAY to the C++ version
                 * 
                 * This will take over the ownership!!!
                 * Type must be correct!!
                 * 
                 * @param inner
                 */
                static SimpleArray<T> Wrap(arr_t inner) {
                    ASSERT_COMPILE_TIME(sizeof(inner) == sizeof(SimpleArray<T>), "SimpleArray<T>::Wrap - size does not match");
                    return CAST_REINTERPRETE_DANGEROUS(SimpleArray<T>, inner);
                }

                static void memcopy(void * dest, const void * src, size_t size) {
                    SIMPLE_ARRAY_MEMCOPY(dest, src, size);
                }
            private:
                T& InternGet(size_t index) const { return SIMPLE_ARRAY_GET(m_arr, index); }
            private:
                arr_t m_arr;
        };

    } // namespace templates
} // namespace m


#endif

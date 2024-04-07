#include "smartbuffer.hpp"

#include "../../smartbuffer.h"

namespace m {
    namespace smart {

        // Buffer

        Buffer::Buffer(size_t size) : m_buffer(sBuffer_create(size)) {}

        Buffer::Buffer(const sBuffer& buf) : Buffer(sBuffer_count_single(&buf)) {
            SMARTBUFFER_FOREACH_V(i, cur, buf,
                this->Add(buffer::SinglePtr(cur));
            );
        } 

        Buffer::Buffer(const Buffer& other) : Buffer(other.Count()) {
            // Guard self assignment
            //if (this != &other) {
                for (buffer::index_t i = 0; i < other.Count(); i++) {
                    this->Add(other.Get(i));
                }
            //}
        }

#if LANG_CPP_STD >= 2011
        Buffer::Buffer(Buffer&& other) : m_buffer(other.m_buffer) {
            // empty other
            other.m_buffer.data = 0;
            other.m_buffer.count = 0;
            other.m_buffer.written = 0;
        }
#endif

        Buffer::~Buffer() { this->Delete(); }

        void Buffer::Delete() { sBuffer_free(&m_buffer); }

        Buffer::index_t Buffer::Count() const { return sBuffer_count_single(&m_buffer); }
        Buffer::size_t Buffer::Length() const { return sBuffer_count(&m_buffer); }

        Buffer::SinglePtr Buffer::Get(buffer::index_t index) const {
            if(this->Count() == 0) { return buffer::SinglePtr(); }
            buffer::SinglePtr ret(sBuffer_get(&m_buffer, index));
            return ret;
        }

        void Buffer::Add(const Buffer::SinglePtr& buf) { sBuffer_add(&m_buffer, buf.GetIntern()); }

#if LANG_CPP_STD >= 2011
        void Buffer::Add(const Buffer::SinglePtr&& buf) {
            sBuffer_add(&m_buffer, buf.GetIntern());
        }
#endif

        void Buffer::Clear() { return sBuffer_clear(&m_buffer); }

        Buffer::size_t Buffer::Read(reader_t readerFunction, size_t length, void * userData) const {
            return sBuffer_read(&m_buffer, readerFunction, length, userData);
        }
//if 0
        Buffer& Buffer::operator=(const Buffer& other) {
            if (this == &other) { return *this; } // Guard self assignment
            this->Delete();
            for (buffer::index_t i = 0; i < other.Count(); i++) {
                this->Add(other.Get(i));
            }
            return *this;
        }

#if LANG_CPP_STD >= 2011
        Buffer& Buffer::operator=(const Buffer&& other) {
            if (this == &other) { return *this; } // Guard self assignment
            this->Delete();
            for (buffer::index_t i = 0; i < other.Count(); i++) {
                this->Add(other.Get(i));
            }
            return *this;
        }
#endif

        Buffer Buffer::operator+(const Buffer::SinglePtr& other) const {
            Buffer ret(this->Length() + other.Length());
            for (buffer::index_t i = 0; i < this->Count(); i++) { ret.Add(this->Get(i)); }
            ret.Add(other);
            return ret;
        }

#if LANG_CPP_STD >= 2011
        Buffer Buffer::operator+(const Buffer::SinglePtr&& other) const {
            Buffer ret(this->Count() + 1);
            for (buffer::index_t i = 0; i < this->Count(); i++) { ret.Add(this->Get(i)); }
            ret.Add(other);
            return ret;
        }
#endif
//#endif
        // private

        void Buffer::UsageIncreaseAll() {
            for (index_t i = 0; i < this->Count(); i++) {
                this->Get(i).UsageCountIncrease();
            }
        }

        void Buffer::UsageDecreaseAll() {
            for (index_t i = 0; i < this->Count(); i++) {
                /*index_t count*/this->Get(i).UsageCountDecrease();
                // need to remove if not static and count == 0
            }
        }

        namespace buffer {

            // SinglePtr

            // public

            SinglePtr::SinglePtr(size_t size) : m_buffer(sBuffer_single_create(size)) { this->m_buffer->own.usage_count = 1; }

            // from internal
            SinglePtr::SinglePtr(const sBuffer_single_ptr& internal) : m_buffer(internal) {
                this->UsageCountIncrease();
            }

    #if LANG_CPP_STD >= 2011
            SinglePtr::SinglePtr(sBuffer_single_ptr&& internal) : m_buffer(internal) {
                this->UsageCountIncrease();
            }
    #endif

            // from other
            SinglePtr::SinglePtr(const SinglePtr& other) : m_buffer(other.m_buffer) {
                this->UsageCountIncrease();
            }

    #if LANG_CPP_STD >= 2011
            SinglePtr::SinglePtr(SinglePtr&& other) : m_buffer(other.m_buffer) {
                this->UsageCountIncrease();
            }
    #endif

            SinglePtr::~SinglePtr() { this->Delete(); }

            SinglePtr SinglePtr::New(size_t size) {
                return SinglePtr(sBuffer_single_create(size));
            }
            SinglePtr SinglePtr::NewOnce(size_t size) {
                return SinglePtr(sBuffer_single_create_once(size));
            }
            SinglePtr SinglePtr::NewReadonly(const char_t * data, size_t len) {
                return SinglePtr(sBuffer_single_create_readonly(data, len));
            }
            SinglePtr SinglePtr::NewUnmanaged(const char_t * data, size_t len, size_t size) {
                return SinglePtr(sBuffer_single_create_unmananged(data, len, size));
            }
            SinglePtr SinglePtr::NewStatic(const char_t * data, size_t len) {
                return SinglePtr(sBuffer_single_create_static(data, len));
            }

            bool SinglePtr::IsNull() const { return (m_buffer == NULL); }
            bool SinglePtr::IsReadOnly() const { return m_buffer->flags.is_readonly; }
            bool SinglePtr::IsStatic() const { return !(m_buffer->flags.is_data_allocated); }
            bool SinglePtr::IsChild() const { return m_buffer->flags.is_child; }

            size_t SinglePtr::Length() const { return sBuffer_single_count(m_buffer); }
            const char_t * SinglePtr::Get() const { return sBuffer_single_get(m_buffer); }

            SinglePtr SinglePtr::Child(index_t begin, size_t size) const {
                return SinglePtr(sBuffer_single_create_child(m_buffer, begin, size));
            }

            char_t * SinglePtr::Release() {
                char_t * data = reinterpret_cast<char_t *>(m_buffer->own.data);
                this->SetNull();
                return data;
            }

            size_t SinglePtr::Write(index_t startIndex, const char_t * data, size_t length) {
                return sBuffer_single_write(m_buffer, startIndex, data, length);
            }
            size_t SinglePtr::Append(const char_t * data, size_t length) {
                return sBuffer_single_add(m_buffer, data, length);
            }
            size_t SinglePtr::Insert(index_t start, const char_t * data, size_t length) {
                return sBuffer_single_insert(m_buffer, start, data, length);
            }

            size_t SinglePtr::Clear() {
                if (!this->IsNull()) {
                    return sBuffer_single_clear(m_buffer);
                }
                return 0;
            }

            void SinglePtr::Delete() {
                if (!this->IsNull()) {
                    sBuffer_single_free(m_buffer);
                    this->SetNull();
                }
            }

            // private

            void SinglePtr::SetNull() { m_buffer = NULL; }

            const sBuffer_single_ptr& SinglePtr::GetIntern() const {
                return m_buffer;
            }

            index_t SinglePtr::UsageCountIncrease() { return sBuffer_single_usageCount_increase(m_buffer); }
            index_t SinglePtr::UsageCountDecrease() { return sBuffer_single_usageCount_decrease(m_buffer); }

        } // namespace buffer

    } // namespace smart
} // namespace m

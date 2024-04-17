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
            if(this->Count() == 0) { return buffer::SinglePtr(nullptr); }
            buffer::SinglePtr ret(sBuffer_get(&m_buffer, index));
            return ret;
        }

        void Buffer::Add(const Buffer::SinglePtr& buf) { sBuffer_add(&m_buffer, buf.GetIntern()); }
        void Buffer::InsertSingle(index_t index, const Buffer::SinglePtr& buf) { sBuffer_insert_single(&m_buffer,index, buf.GetIntern()); }

#if LANG_CPP_STD >= 2011
        void Buffer::Add(Buffer::SinglePtr&& buf) {
            sBuffer_add(&m_buffer, Buffer::SinglePtr(buf).GetIntern());
        }
        void Buffer::InsertSingle(index_t index, Buffer::SinglePtr&& buf) {
            sBuffer_insert_single(&m_buffer,index, Buffer::SinglePtr(buf).GetIntern());
        }
#endif

        Buffer::size_t Buffer::Insert(index_t index, const char_t * data, size_t len) { return sBuffer_insert(&m_buffer, index, data, len); }
        Buffer::size_t Buffer::Append(const char_t * data, size_t len) { return sBuffer_append(&m_buffer, data, len); }

        void Buffer::Clear() { return sBuffer_clear(&m_buffer); }

        Buffer::size_t Buffer::Read(const Iterator& pos, size_t length, reader_t readerFunction, void * userData) const {
            return sBuffer_read(&m_buffer, &pos.m_current, length, readerFunction, userData);
        }
        Buffer::size_t Buffer::ReadTo(char_t * dest, const Iterator& pos, size_t length) const {
            return sBuffer_read_to(dest, &m_buffer, &pos.m_current, length);
        }
        Buffer::size_t Buffer::ReadTo(SinglePtr& dest, const Iterator& pos) const {
            const size_t written = this->ReadTo(const_cast<Buffer::char_t *>(dest.Get()), pos, dest.Capacity());
            dest.m_buffer->len = written;
            return written;
        }

        Buffer::Iterator Buffer::begin() const {
            return Iterator::Start(*this);
        }
        Buffer::Iterator Buffer::end() const {
            return Iterator::End();
        }

        Buffer::Iterator Buffer::FindIndex(index_t index) const {
            return Iterator::AtIndex(*this, index);
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
        Buffer& Buffer::operator=(Buffer&& other) {
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
        Buffer Buffer::operator+(Buffer::SinglePtr&& other) const {
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

            SinglePtr::SinglePtr(decltype(nullptr)) : m_buffer(nullptr) {}

            // from internal
            SinglePtr::SinglePtr(const sBuffer_single_ptr& internal) : m_buffer(internal) {
                this->UsageCountIncrease();
            }

    #if LANG_CPP_STD >= 2011
            SinglePtr::SinglePtr(sBuffer_single_ptr&& internal) : m_buffer(internal) {
                if (sBuffer_single_usageCount_get(internal) == 0) { sBuffer_single_usageCount_increase(internal); }
                internal = nullptr;
            }
    #endif

            // from other
            SinglePtr::SinglePtr(const SinglePtr& other) : m_buffer(other.m_buffer) {
                this->UsageCountIncrease();
            }

    #if LANG_CPP_STD >= 2011
            SinglePtr::SinglePtr(SinglePtr&& other) : m_buffer(other.m_buffer) {
                other.m_buffer = nullptr;
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
            size_t SinglePtr::Capacity() const { return sBuffer_single_size(m_buffer); }
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
 
            char_t SinglePtr::operator[](index_t index) {
                const char_t * s = sBuffer_single_get(m_buffer);
                return s[index];
            }

            // private

            void SinglePtr::SetNull() { m_buffer = NULL; }

            const sBuffer_single_ptr& SinglePtr::GetIntern() const {
                return m_buffer;
            }

            index_t SinglePtr::UsageCountIncrease() { return sBuffer_single_usageCount_increase(m_buffer); }
            index_t SinglePtr::UsageCountDecrease() { return sBuffer_single_usageCount_decrease(m_buffer); }

            // Iterator

            Iterator::Iterator(const sBuffer * buffer) : m_buffer(buffer) {}
            Iterator::Iterator(const sBuffer * buffer, const sBuffer_index_descr& indexDescr) : m_buffer(buffer), m_current(indexDescr) {}

            Iterator Iterator::Start(const Buffer& buffer) { return Iterator(&buffer.m_buffer, GetStartDescr(&buffer.m_buffer)); }
            Iterator Iterator::AtIndex(const Buffer& buffer, index_t index) { return Iterator(&buffer.m_buffer, sBuffer_find_index(&buffer.m_buffer, index)); }
            Iterator Iterator::End() { return Iterator(nullptr); }

            Iterator& Iterator::Next() {
                // Do not go further if we are at the end
                if (this->IsLastElement()) {
                    this->SetEnd();
                    return *this;
                }
                if (this->AtEndOfSingle()) {
                    // Go to next buffer
                    return this->NextSingle();
                }
                // else only go further in current buffer
                ++m_current.single.index;
                return *this;
            }

            Iterator& Iterator::Advance(size_t len) {
                // Do not go further if we are at the end
                if (this->IsLastElement()) {
                    this->SetEnd();
                    return *this;
                }

                for (size_t advanced = this->AdvanceSingle(len); advanced < len; advanced = this->AdvanceSingle(len)) {
                    len -= advanced; // how much we still have to advance
                    NextSingle();
                    len--; // because we went to the next single buffer
                }
                return *this;
            }

            Iterator& Iterator::GoToStart() {
                m_current = GetStartDescr(m_buffer);
                return *this;
            }

            Iterator& Iterator::GoToIndex(index_t index) {
                m_current = sBuffer_find_index(m_buffer, index);
                return *this;
            }

            Iterator& Iterator::GoToLast() {
                m_current = GetLastDescr(m_buffer);
                return *this;
            }

            Iterator& Iterator::SetEnd() {
                m_buffer = nullptr;
                return *this;
            }

            char_t Iterator::Get() const {
                return sBuffer_single_get(*m_current.single.bufP)[m_current.single.index];
            }

            bool Iterator::IsLastElement() const {
                return m_current.single.index + 1 >= sBuffer_count_single(m_buffer) && this->AtEndOfSingle();
            }

            bool operator==(const Iterator& it1, const Iterator& it2) {
                // if one of them is end, then both must be end
                if (it1.m_buffer == nullptr || it2.m_buffer == nullptr) {
                    return it1.m_buffer == it2.m_buffer;
                }
                return
                    &it1.m_buffer == &it2.m_buffer &&
                    it1.m_current.index == it2.m_current.index &&
                    it1.m_current.single.index == it2.m_current.single.index
                ;
            }

            // private methods

            bool Iterator::AtEndOfSingle() const {
                return m_current.single.index + 1 >= sBuffer_single_count(*m_current.single.bufP);
            }

            Iterator& Iterator::NextSingle() {
                m_current.single.bufP = sBuffer_getP(m_buffer, (++m_current.index));
                m_current.single.index = 0;
                return *this;
            }

            size_t Iterator::AdvanceSingle(size_t len) {
                const size_t maxlen = sBuffer_single_count(*m_current.single.bufP);
                const size_t advanced = len <= maxlen ? len : maxlen;
                m_current.single.index += advanced;
                return advanced;
            }

            sBuffer_index_descr Iterator::GetStartDescr(const sBuffer * buf) {
                const sBuffer_single_with_index inner = { sBuffer_getP(buf, 0), 0};
                return (sBuffer_index_descr) { inner, 0 };
            }
            sBuffer_index_descr Iterator::GetLastDescr(const sBuffer * buf) {
                sBuffer_index_descr descr;
                descr.index = sBuffer_count_single(buf) - 1;
                descr.single.bufP = sBuffer_getP(buf, descr.index);
                descr.single.index = sBuffer_single_count(*descr.single.bufP) - 1;
                return descr;
            }

        } // namespace buffer

    } // namespace smart
} // namespace m

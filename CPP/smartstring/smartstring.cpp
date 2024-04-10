#include "../../templates/def.h"
#include "smartstring.hpp"

//#include "../../src/words.h"
//#include "../../src/word_pick.h"
#include "../../words_h.h"
#include "../../word_pick_h.h"

#include "../../smartbuffer.h"
#include "../../src/smartbuffer/smartbuffer_intern.h"

smart::string::String::String() : m_str(sString_create()) {}
smart::string::String::String(const char * cstr) : m_str(sString_init((sString_char_t *) cstr, word_len(cstr), SMARTSTRING_FLAG_U_ZERO)) {}
smart::string::String::String(smart::string::String::char_t * str, smart::string::String::len_t len, smart::string::Flags f) : m_str(sString_init(str, len, f.GetInner())) {}

smart::string::String::String(const ::sString& inner) : m_str(inner) {
    sBuffer_usageCount_increaseAll(SSTRING_GETBUF_ADDR_V((const_cast< ::sString&>(inner))));
}
smart::string::String::String(const String& other) : m_str(other.m_str) {
    sBuffer_usageCount_increaseAll(SSTRING_GETBUF_ADDR_V(const_cast< ::sString&>(other.m_str)));
}
smart::string::String::String(String&& other) : m_str(other.m_str) {
    // empty other
    other.m_str.buf.data = 0;
    other.m_str.buf.count = 0;
    other.m_str.buf.written = 0;
}
smart::string::String::~String() {
    sBuffer_free(SSTRING_GETBUF_ADDR_V(m_str));
}

// Array functions
smart::string::String smart::string::String::Get(smart::string::String::index_t i) { return smart::string::String(sString_get(&m_str, i)); }
smart::string::String::len_t smart::string::String::Count() const { return sString_count(&m_str); }

smart::string::String& smart::string::String::Add(const smart::string::String& s) {
    ::sString_add(&m_str, s.m_str);
    return *this;
}

smart::string::String& smart::string::String::Append(const char_t * str, len_t len) {
    ::sString_append(&m_str, str, len);
    return *this;
}

smart::string::String& smart::string::String::Append(const char * str) {
    return this->Append((const char_t *)str, word_len(str));
}

// String functions

smart::string::String::len_t smart::string::String::Length() const { return sString_len(&m_str); }

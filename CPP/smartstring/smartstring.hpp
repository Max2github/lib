#ifndef SMARTSTRING_HPP
#define SMARTSTRING_HPP

#include "../../freestanding/def.h"
#include "../../templates/simple_array.h"

#ifndef NO_STD_LIB
    #include<stdlib.h>

    #ifndef SMARTSTRING_HPP_MALLOC
    #define SMARTSTRING_HPP_MALLOC(size) malloc(size)
    #endif

    #ifndef SMARTSTRING_HPP_REALLOC
    #define SMARTSTRING_HPP_REALLOC(oldP, size, oldSize) realloc(oldP, size, oldSize)
    #endif

    #ifndef SMARTSTRING_HPP_FREE
    #define SMARTSTRING_HPP_FREE(p) free(p)
    #endif
#endif

#undef SMARTSTRING_H_MALLOC
#undef SMARTSTRING_H_REALLOC
#undef SMARTSTRING_H_FREE
#define SMARTSTRING_H_MALLOC  SMARTSTRING_HPP_MALLOC
#define SMARTSTRING_H_REALLOC SMARTSTRING_HPP_REALLOC
#define SMARTSTRING_H_FREE SMARTSTRING_HPP_FREE

//typedef struct sString sString;

#include "../../smartstring.h"

namespace smart {
    namespace string {

        class Flags {
            public:
                typedef index8 byte_t;
                class Event {
                    public:
                        inline Event(sString_flags_u * flags) : add(flags) {}

                        class Add {
                            private:
                                sString_flags_u * m_flags;
                            public:
                                inline Add(sString_flags_u * flags) : m_flags(flags) {};

                                inline bool Join() const { return m_flags->obj.on_add_join; };
                                inline void Join(bool set) { m_flags->obj.on_add_join = set; };

                                inline bool Copy() const { return m_flags->obj.on_add_copy; };
                                inline void Copy(bool set) { m_flags->obj.on_add_copy = set; };
                        };

                    private:
                        Add add;
                };
            public:
                inline Flags() : on(&m_flags) { m_flags.byte = 0; };
                inline Flags(byte_t byte) : on(&m_flags) { m_flags.byte = byte; };

                inline byte_t GetByte() const { return m_flags.byte; };
                inline sString_flags_u GetInner() const { return m_flags; };

                inline bool IsConst() const { return m_flags.obj.is_const; }
                inline bool IsStatic() const { return m_flags.obj.is_static; }

                Flags& operator|(const Flags& other) { m_flags.byte |= other.GetByte(); return *this; }
                Flags& operator|(const byte_t& byte) { m_flags.byte |= byte; return *this; }

                Flags& operator&(const Flags& other) { m_flags.byte &= other.GetByte(); return *this; }
                Flags& operator&(const byte_t& byte) { m_flags.byte &= byte; return *this; }
            public:
                Event on;
            private:
                sString_flags_u m_flags;
        };


        class String {
            public:
                typedef sString_bool_t bool_t;
                typedef sString_char_t char_t;
                typedef sString_len_t len_t;
                //typedef sString_index_t index_t;
                typedef len_t index_t;
            public:
                String();

                /**
                 * @brief Construct a new sString object from a c string
                 * 
                 */
                explicit String(const char *);
                String(char_t *, len_t, Flags f = Flags());
                explicit String(const ::sString&);
                #if LANG_CPP_STD >= 2011
                inline String(const ::sString&& inner)  : m_str(inner) {}
                #endif
                String(const String&); // copy
                String(String&&);
                ~String();

                //class Array {
                    //public:
                        String Get(index_t);
                        len_t Count() const;

                        template<typename... args_t>
                        using ForEachFunc_t = void (*)(const String&, args_t...);

                        template<typename... args_t>
                        inline void ForEach(ForEachFunc_t<args_t...> func, args_t... args) {
                            for (index_t i = 0; i < this->Count(); i++) {
                                func(this->Get(i), args...);
                            }
                        }

                        template<typename Func, typename... args_t>
                        inline void ForEach(Func&& func, args_t... args) {
                            for (index_t i = 0; i < this->Count(); i++) {
                                func(this->Get(i), args...);
                            }
                        }

                        inline String operator[](index_t index) { return this->Get(index); }
                //};

                len_t Length() const;

                #ifdef SMARTBUFFER_HPP
                inline m::smart::Buffer GetBuffer() { return m::smart::Buffer(m_str.buf); }
                #endif

                String& Add(const String&);
                String& Append(const char_t *, len_t);
                String& Append(const char *);

                inline String& operator+=(const String& other) { this->Add(other); return *this; }

                template<typename stream>
                const String& operator>>(stream& out) const {
                    sString_readHandler func = [] (void * o, const char_t * data, len_t len) {
                        stream& sout = *((stream *) o);
                        for (len_t i = 0; i < len; i++) {
                            sout << (char) *data;
                            data++;
                        }
                    };
                    sBuffer_read(SSTRING_GETBUF_ADDR_V(m_str), NULL, this->Length(), func, (void *) &out);
                    return *this;
                }

            private:
                ::sString m_str;
        };

    } // namespace smart
} // namespace string

#endif //! SMARTSTRING_HPP
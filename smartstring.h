#ifndef SMARTSTRING_H
#define SMARTSTRING_H

#include "templates/def.h"

#ifndef NO_STD_LIB
    #include<stdlib.h>

    #define SMARTSTRING_H_MALLOC(size) malloc(size)
    #define SMARTSTRING_H_REALLOC(oldP, size) realloc(oldP, size)
    #define SMARTSTRING_H_FREE(p) free(p)
#endif

#ifndef SMARTBUFFER_CUSTOM
#define SMARTBUFFER_CUSTOM
#define SMARTBUFFER_CHAR char // index8
#define SMARTBUFFER_LEN_T index64
#define SMARTBUFFER_BOOL_T bool
#define SMARTBUFFER_TRUE true
#define SMARTBUFFER_FALSE false
#endif

#undef SMARTBUFFER_H_MALLOC
#undef SMARTBUFFER_H_REALLOC
#undef SMARTBUFFER_H_FREE
#define SMARTBUFFER_H_MALLOC  SMARTSTRING_H_MALLOC
#define SMARTBUFFER_H_REALLOC SMARTSTRING_H_REALLOC
#define SMARTBUFFER_H_FREE    SMARTSTRING_H_FREE

// #include "/usr/local/Cellar/icu4c/72.1/include/unicode/utf8.h"
#include "smartbuffer.h"

struct sString_flags_s {
    bool is_const : 1;
    bool is_static : 1;
    bool on_add_join : 1; // if on adding strings they should be joined (more memory copied), or if they should just be appended
    bool on_add_copy : 1; // if, when another string is added, the data should be copied (default: augment usage count)
};
typedef struct sString_flags_s sString_flags_s;

typedef unsigned char sString_flags_b;

union sString_flags_u {
    sString_flags_b byte;
    sString_flags_s obj;
};
typedef union sString_flags_u sString_flags_u;

#if 0
enum sString_flag_e {
    // struct
    S_ZERO,
    S_CONST,
    S_STATIC,
    S_JOIN,

    S_STATIC_CONST,

    // byte
    B_ZERO,
    B_CONST,
    B_STATIC,
    B_JOIN,

    B_STATIC_CONST,

    // union
    U_ZERO,
    U_CONST,
    U_STATIC,
    U_JOIN,

    U_STATIC_CONST
};
//#endif

#define SSTRING_FLAG(flag_e) SMARTSTRING_FLAG_##flag_e
#define SSTRING_FLAG_U(flag_e_b) SMARTSTRING_FLAG_U(SSTRING_FLAG(flag_e_b))
#endif

#define SSTRING_FLAG_U(flag_b) SMARTSTRING_FLAG_U(flag_b)

// struct
#define SMARTSTRING_FLAG_S_ZERO   (sString_flags_s) { 0, 0, 0 }
#define SMARTSTRING_FLAG_S_CONST  (sString_flags_s) { 1, 0, 0 }
#define SMARTSTRING_FLAG_S_STATIC (sString_flags_s) { 0, 1, 0 }
#define SMARTSTRING_FLAG_S_JOIN   (sString_flags_s) { 0, 0, 1 }

#define SMARTSTRING_FLAG_S_STATIC_CONST (sString_flags_s) { 1, 1, 0 }

// byte - can be used with OR |
#define SMARTSTRING_FLAG_B_ZERO   0
#define SMARTSTRING_FLAG_B_CONST  1
#define SMARTSTRING_FLAG_B_STATIC 2
#define SMARTSTRING_FLAG_B_JOIN   4

#define SMARTSTRING_FLAG_B_STATIC_CONST SMARTSTRING_FLAG_B_CONST | SMARTSTRING_FLAG_B_STATIC

// union
#define SMARTSTRING_FLAG_U(flagB) (sString_flags_u) { flagB }
#define SMARTSTRING_FLAG_U_ZERO   SMARTSTRING_FLAG_U( SMARTSTRING_FLAG_B_ZERO   )
#define SMARTSTRING_FLAG_U_CONST  SMARTSTRING_FLAG_U( SMARTSTRING_FLAG_B_CONST  )
#define SMARTSTRING_FLAG_U_STATIC SMARTSTRING_FLAG_U( SMARTSTRING_FLAG_B_STATIC )
#define SMARTSTRING_FLAG_U_JOIN   SMARTSTRING_FLAG_U( SMARTSTRING_FLAG_B_JOIN   )

#define SMARTSTRING_FLAG_U_STATIC_CONST SMARTSTRING_FLAG_U( SMARTSTRING_FLAG_B_STATIC_CONST )

struct sString {
    sBuffer buf;
    sString_flags_u flags;
};
typedef struct sString sString;

#define SSTRING_GETBUF_ADDR_P(s) (&(s->buf))
#define SSTRING_GETBUF_ADDR_V(s) (&(s.buf))

typedef SMARTBUFFER_CHAR sString_char_t;
typedef SMARTBUFFER_LEN_T sString_len_t;
typedef SMARTBUFFER_BOOL_T sString_bool_t;

sString sString_create();
sString sString_init(sString_char_t *, sString_len_t, sString_flags_u);

sString_len_t sString_count(const sString *);
sString_len_t sString_len(const sString *);

void sString_addSingleBuf(sString *, sBuffer_single_ptr);
void sString_add(sString *, sString);
void sString_addStr(sString *, const sString_char_t *, sString_len_t);

/**
 * @brief Get an element of an sString by index
 * The sString is treated as an array
 * \param 0 (const sString *) the array
 * \param 1 (sString_len_t) the index of the element to get
 * 
 * Attention! element != character !!!
 * An sSting can also be an array of strings and thus returns a string (sString)
 * 
 * @return a static const sString that just points to an element of the sString
 */
sString sString_get(const sString *, sString_len_t);
/**
 * @brief Get a part of the sString
 * @param 0 (const sString *) the sString (read as string)
 * @param 1 (sString_len_t) at which character the substring should start (inclusive)
 * @param 2 (sString_len_t) how long the substring should be
 * 
 * @return a sString (elements are static const) that just points to a part of the sString 
 */
sString sString_subStr(const sString *, sString_len_t, sString_len_t);

char * sString_toCstr(sString *);

#endif // !SMARTSTRING_H
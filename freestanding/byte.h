#ifndef BIT_H
#define BIT_H

#include "def.h"

typedef unsigned char byte_t;

struct byte_s {
    union {
        struct {
            byte_t first : 1;
            byte_t second : 1;
            byte_t third : 1;
            byte_t fourth : 1;
            byte_t fifth : 1;
            byte_t sixth : 1;
            byte_t seventh : 1;
            byte_t eighth : 1;
        };
        struct {
            byte_t b1 : 1;
            byte_t b2 : 1;
            byte_t b3 : 1;
            byte_t b4 : 1;
            byte_t b5 : 1;
            byte_t b6 : 1;
            byte_t b7 : 1;
            byte_t b8 : 1;
        };
    };
};
typedef struct byte_s byte_s;

#define BYTE_CHECK_SIZE(byte) ASSERT_COMPILE_TIME(sizeof(byte) == 1)
#define BYTE_BYTE_TO_STRUCT(byte) CAST_REINTERPRETE_DANGEROUS(byte_s, byte)
#define BYTE_STRUCT_TO_BYTE(obje) CAST_REINTERPRETE_DANGEROUS(byte_t, obje)

#define BYTE_SET_BIT_1(byte) (byte | 0x01) // 128 - 10 000 000
#define BYTE_SET_BIT_2(byte) (byte | 0x02) // 64  - 1 000 000
#define BYTE_SET_BIT_3(byte) (byte | 0x04) // 32  - 100 000
#define BYTE_SET_BIT_4(byte) (byte | 0x08) // 16  - 10 000
#define BYTE_SET_BIT_5(byte) (byte | 0x10) // 8   - 1 000
#define BYTE_SET_BIT_6(byte) (byte | 0x20) // 4   - 100
#define BYTE_SET_BIT_7(byte) (byte | 0x40) // 2   - 10
#define BYTE_SET_BIT_8(byte) (byte | 0x80) // 1   - 1

#define BYTE_SET_BIT_FIRST(byte)   BYTE_SET_BIT_1(byte)
#define BYTE_SET_BIT_SECOND(byte)  BYTE_SET_BIT_2(byte)
#define BYTE_SET_BIT_THIRD(byte)   BYTE_SET_BIT_3(byte)
#define BYTE_SET_BIT_FOURTH(byte)  BYTE_SET_BIT_4(byte)
#define BYTE_SET_BIT_FIFTH(byte)   BYTE_SET_BIT_5(byte)
#define BYTE_SET_BIT_SIXTH(byte)   BYTE_SET_BIT_6(byte)
#define BYTE_SET_BIT_SEVENTH(byte) BYTE_SET_BIT_7(byte)
#define BYTE_SET_BIT_EIGHTH(byte)  BYTE_SET_BIT_8(byte)

#define BYTE_SET_BIT(byte, index) BIT_SET_BIT_##index(##byte)
#define BYTE_SET(data, index) (data | 1 << index)

// verkehrt herum
#define BYTE_GET_BIT_1(byte) ((byte & 0x7F) >> 7) // 127 - 1 111 111
#define BYTE_GET_BIT_2(byte) ((byte & 0x3F) >> 6) // 63  - 111 111
#define BYTE_GET_BIT_3(byte) ((byte & 0x1F) >> 5) // 31  - 11 111
#define BYTE_GET_BIT_4(byte) ((byte & 0x0F) >> 4) // 15  - 1 111
#define BYTE_GET_BIT_5(byte) ((byte & 0x07) >> 3) // 7   - 111
#define BYTE_GET_BIT_6(byte) ((byte & 0x03) >> 2) // 3   - 11
#define BYTE_GET_BIT_7(byte) ((byte & 0x01) >> 1) // 1   - 1
#define BYTE_GET_BIT_8(byte) ((byte & 0x00) >> 0) // 0   - 0

#define BYTE_GET_BIT_FIRST(byte)   BYTE_GET_BIT_1(byte)
#define BYTE_GET_BIT_SECOND(byte)  BYTE_GET_BIT_2(byte)
#define BYTE_GET_BIT_THIRD(byte)   BYTE_GET_BIT_3(byte)
#define BYTE_GET_BIT_FOURTH(byte)  BYTE_GET_BIT_4(byte)
#define BYTE_GET_BIT_FIFTH(byte)   BYTE_GET_BIT_5(byte)
#define BYTE_GET_BIT_SIXTH(byte)   BYTE_GET_BIT_6(byte)
#define BYTE_GET_BIT_SEVENTH(byte) BYTE_GET_BIT_7(byte)
#define BYTE_GET_BIT_EIGHTH(byte)  BYTE_GET_BIT_8(byte)

#define BYTE_BIT_GET_BIT(byte, index) BIT_GET_BIT_##index(byte)
#define BYTE_BIT_IS_SET(data, index) (data >> index)

#endif
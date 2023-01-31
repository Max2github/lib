/**
 * @file format_options.h
 * @author Max2
 * @brief little library for printing formatted text
 * 
 * This library is used to pass options needed for text formatting
 * like:
 * - multi line or single line
 * - tabsize
 * - offset
 * - allow (unneeded) spaces
 */

#ifndef FORMAT_OPTIONS_H
#define FORMAT_OPTIONS_H

#include "def.h"

struct format_options {
    bool multi_line;
    bool allow_spaces;
    index8 tabsize;
    index8 offset;
};
// object
typedef struct format_options format_options;
typedef const format_options format_options_readonly;
// pointer
typedef format_options * format_options_p;
typedef format_options_readonly * format_options_readonly_p;

#define FORMAT_OPTIONS_DEFAULT_HUMAN  { true,  true,  4, 0 }
#define FORMAT_OPTIONS_DEFAULT_PACKED { false, false, 0, 0 }
#define FORMAT_OPTIONS_DEFAULT FORMAT_OPTIONS_DEFAULT_HUMAN

#define FORMAT_OPTIONS_SUM_ADD(f, ntabs) ((f).offset + ((ntabs) * (f).tabsize))
#define FORMAT_OPTIONS_SUM_ADD_P(f_p, ntabs) FORMAT_OPTIONS_SUM_ADD((*f_p), (ntabs))

#define FORMAT_OPTIONS_SUM(f) ((f).offset)
#define FORMAT_OPTIONS_SUM_P(f) FORMAT_OPTIONS_SUM((*f))

#define FORMAT_OPTIONS_TAB(f, ntabs) ((f).offset += (f).tabsize)
#define FORMAT_OPTIONS_TAB_P(f_p, ntabs) FORMAT_OPTIONS_TAB((*f_p), (ntabs))

#define FORMAT_OPTIONS_BACK(f, ntabs) ((f).offset -= (f).tabsize)
#define FORMAT_OPTIONS_BACK_P(f_p, ntabs) FORMAT_OPTIONS_BACK((*f_p), (ntabs))

// #define FORMAT_OPTIONS_

#endif
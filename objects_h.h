#ifndef OBJECTS_H_H
#define OBJECTS_H_H

#include "list_h.h"

#define Object (list_type) (End+1)

typedef list object;
typedef list_element_pointer object_element_pointer;

void object_element_free(object_element_pointer);
void object_free(object);

object object_add(object target, const char * name, ...);
object object_appendChild(object target, const char * name, object child);

object object_copy(object target);

void object_print(object target, unsigned int format);

object_element_pointer object_get_el(object target, const char * name);
object_element_pointer object_get(object target, const char * path);

object object_del_el(object target, const char * name);
object object_del(object target, const char * path);

void object_mod_el(object target, const char * name, list_type type, unsigned long long value);
void object_mod(object target, const char * path, list_type type, unsigned long long value);

object object_JSON_read_one(object json_as_obj, const char * prep_json_string);
object object_JSON_read(const char * json_string);
void object_JSON_stringify(char * dest, object obj, bool multi_line, int8 tabsize, int8 format);

#endif
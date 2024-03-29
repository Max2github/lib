#include "../objects_h.h"

#include "list.h"

void object_element_free(object_element_pointer head) {
    if (head->type == List || head->type == Object) {
        object_free((list) head->el);
    } else if (head->type == String) {
        LIST_H_FREE((void *) head->el);
    }
    LIST_H_FREE(head);
}
void object_free(object target) {
    while (target != NULL) {
        list next = target->next;
        object_element_free(target);
        target = next;
    }
}

object object_add(object target, const char * name, ...) {
    va_list args;
    va_start(args, name);
    list newEntry = new_list_element_as_list(args);
    va_end(args);
    if (list_node_len(newEntry) == 1) {
        newEntry = list_addFirst(newEntry, String, name, End);
        target = list_addFirst(target, List, newEntry, End);
    } else {
        target = list_addFirst(target, String, name, List, newEntry, End);
    }
    list_free(newEntry);
    return target;
}
object object_appendChild(object target, const char * name, object child) {
    target = list_addFirst(target, String, name, List_pointer, child, End);
    ((list) target->el)->next->type = Object;
    return target;
}
object object_copy(object target) {
    object Copy = NULL;
    while(target != NULL) {
        char * name = (char * ) ((list) target->el)->el;
        list_element_pointer Inhalt = ((list) target->el)->next;

        if (Inhalt->type == Object) {
            Copy = object_appendChild(Copy, name, object_copy((object) Inhalt->el));
        }
        else {
            list_element_pointer copy_el = new_list_element_type(Inhalt->type, Inhalt->el);
            list newEntry = list_addFirst_element(NULL, copy_el);
            newEntry = list_addFirst(newEntry, String, name, End);
            Copy = list_addFirst(Copy, List, newEntry, End);
        }
        target = target->next;
    }
    return Copy;
}

void object_print(object target, format_options f) {
    // if (target == NULL) { puts("NULL"); return; }
    LIST_H_PUTCHAR('{'); LIST_H_PUTCHAR('\n');

    FORMAT_OPTIONS_TAB(f, 1);
    while(target != NULL) {
        char * name = (char * ) ((list) target->el)->el;
        list_element_pointer Inhalt = ((list) target->el)->next;

        list_print_format(FORMAT_OPTIONS_SUM(f)); LIST_H_PRINTF("%s : ", name);
        if (Inhalt->type == Object) { object_print((object) Inhalt->el, f); }
        else {
            if (Inhalt->type == String) { LIST_H_PUTCHAR('\"'); }
            list_element_print(Inhalt, f);
            if (Inhalt->type == String) { LIST_H_PUTCHAR('\"'); }
            if (Inhalt->type != List && Inhalt->type != List_pointer) { LIST_H_PUTCHAR('\n'); }
        }
        target = target->next;
    }
    FORMAT_OPTIONS_BACK(f, 1);
    list_print_format(FORMAT_OPTIONS_SUM(f));
    LIST_H_PUTCHAR('}'); LIST_H_PUTCHAR('\n');
}
void object_element_print(object target, format_options f) {
    if (target->type == Object) { object_print((object) target->el, f); }
    else { list_element_print((list_element_pointer) target, f); }
}

object_element_pointer object_get_el(object target, const char * name) {
    while (target != NULL) {
        char * tempName = (char * ) ((list) target->el)->el;
        list_element_pointer Inhalt = ((list) target->el)->next;
        if (word_compare(name, tempName) == 0) {
            return Inhalt;
        }
        target = target->next;
    }
    return NULL;
}
object_element_pointer object_get(object target, const char * path) {
    if (find(path, ".")) {
        const unsigned int nameLen = word_len_until(path, ".");
        char name[nameLen+1];
        path = word_copy_until(name, path, ".");
        object_element_pointer underObj = object_get_el(target, name);
        if (underObj->type != Object) { return NULL; }
        return object_get((object) underObj->el, path);
    }
    return object_get_el(target, path);
}
object object_del_el(object target, const char * name) {
    object before = NULL;
    object urTarget = target;
    while (target != NULL) {
        char * tempName = (char * ) ((list) target->el)->el;
        // list_element_pointer Inhalt = ((list) target->el)->next;
        if (word_compare(name, tempName) == 0) {
            if (before == NULL) {
                object next = target->next;
                object_element_free(target);
                return next;
            }
            before->next = target->next;
            object_element_free(target);
            return urTarget;
        }
        
        before = target;
        target = target->next;
    }
    return urTarget;
}
object object_del(object target, const char * path) {
    if (find(path, ".")) {
        object underObj = target; // holds the current object
        object_element_pointer underObj_el = NULL; // holds the current object_element, which holds "underObj" -> Also has type and name of "underObj"
        while (find(path, ".")) {
            const unsigned int nameLen = word_len_until(path, ".");
            char name[nameLen+1];
            path = word_copy_until(name, path, ".");

            underObj = object_get_el(underObj, name);
            if (underObj->type != Object) { LIST_H_PRINTF("!! ERROR: Could not delete %s from object", path); return target; }
            underObj_el = underObj;
            underObj = (object) underObj->el;
        }
        underObj_el->el = (unsigned long long) object_del_el(underObj, path);
        return target;
    }
    return object_del_el(target, path);
}
void object_mod_el(object target, const char * name, list_type type, unsigned long long value) {
    while (target != NULL) {
        char * tempName = (char * ) ((list) target->el)->el;
        if (word_compare(name, tempName) == 0) {
            list_element_pointer Inhalt = ((list) target->el)->next;
            list_element_free(Inhalt);
            Inhalt = new_list_element_type(type, value);
            ((list) target->el)->next = Inhalt;
            return;
        }
        target = target->next;
    }
    return;
}
void object_mod(object target, const char * path, list_type type, unsigned long long value) {
    if (find(path, ".")) {
        object underObj = target; // holds the current object
        object_element_pointer underObj_el = NULL; // holds the current object_element, which holds "underObj" -> Also has type and name of "underObj"
        while (find(path, ".")) {
            const unsigned int nameLen = word_len_until(path, ".");
            char name[nameLen+1];
            path = word_copy_until(name, path, ".");

            underObj = object_get_el(underObj, name);
            if (underObj == NULL) { LIST_H_PRINTF("!! ERROR: Could not modify %s from object", path); return; }
            if (underObj->type != Object) { LIST_H_PRINTF("!! ERROR: Could not modify %s from object", path); return; }
            underObj_el = underObj;
            underObj = (object) underObj->el;
        }
        object_mod_el(underObj, path, type, value);
        return;
    }
    object_mod_el(target, path, type, value);
}

object object_JSON_read_one(object json_as_obj, const char * prep_json_string) {
    if (*prep_json_string == '\"') { prep_json_string++; }
    const unsigned int nameLen = word_len_until(prep_json_string, ":");
    char name[nameLen+1];
    const char * content = word_copy_until(name, prep_json_string, ":");
    if (name[nameLen-1] == '\"') { name[nameLen-1] = '\0'; }
    switch (*content) {
        case '{': {
            content++;
            // not using word_len_until(content, "}"), because underObjLen could be longer (see while below)
            const unsigned int underObjLen = word_len(content);
            char underObjStr[underObjLen+1];
            content = word_copy_until(underObjStr, content, "}");

            char * underPointer = underObjStr;
            while (find(underPointer, "{")) {
                word_add(underPointer, "}");
                underPointer = underObjStr + word_len(underObjStr);
                content = word_copy_until(underPointer, content, "}");
                underPointer = underObjStr + word_len(underObjStr);
                word_add(underPointer, "}");
                content++;
            }

            object underObj = object_JSON_read_one(NULL, underObjStr);
            json_as_obj = object_appendChild(json_as_obj, name, underObj);

            if (*content == ',') { json_as_obj = object_JSON_read_one(json_as_obj, content+1); }
            break;
        }
        case '[': { break; }
        case '\"': {
            content++;
            const unsigned int strLen = word_len_until(content, "\"");
            char str[strLen+1];
            content = word_copy_until(str, content, "\"");

            json_as_obj = object_add(json_as_obj, name, String, str, End);
            if (*content == ',') { json_as_obj = object_JSON_read_one(json_as_obj, content+1); }
            break; 
        }
        default: {
            if (find(content, ".")) {

            } else {
                int num = 0;
                for (; *content != ',' && *content != '}' && *content != '\0'; content++) {
                    num *= 10; num += (*content) - 48;
                }
                json_as_obj = object_add(json_as_obj, name, Integer, num, End);
                if (*content == ',') { json_as_obj = object_JSON_read_one(json_as_obj, content+1); }
            }
            break;
        }
    }
    return json_as_obj;
}
object object_JSON_read(const char * json_string) {
    char ** wordArr;
    int Teile = split(json_string, "\"", &wordArr);
    for (int i = 1; i < Teile; i += 2) {
        replace(wordArr[i], "_", "\7");
        replace(wordArr[i], " ", "_");
    }
    char mod_json_string[word_len(json_string)];
    join((const char **) wordArr, Teile, "\"",mod_json_string);
    freeWordArr(wordArr, Teile);

    replace(mod_json_string, "\n", " ");
    replaceS(mod_json_string, " ", "");
    replace(mod_json_string, "_", " ");
    replace(mod_json_string, "\7", "_");

    object json_as_obj = NULL;
    char * derzeitJSON = mod_json_string + 1;

    return object_JSON_read_one(json_as_obj, derzeitJSON);
}

char * object_sprint_format(char * dest, int8 format) {
    for (int8 i = 0; i < format; i++) { *dest++ = ' '; }
    return dest;
}

void object_JSON_stringify(char * dest, object obj, format_options f) {
    #define OBJECT_JSON_STRINGIFY_PUTCHAR(c) (*dest++ = c); (*dest = '\0')
    #define OBJECT_JSON_STRINGIFY_RESETPOS() (dest += word_len(dest));
    #define OBJECT_JSON_STRINGIFY_PUTS(str) word_add(dest, str); OBJECT_JSON_STRINGIFY_RESETPOS()

    *dest = '\0';
    OBJECT_JSON_STRINGIFY_PUTCHAR('{');
    if (f.multi_line) { OBJECT_JSON_STRINGIFY_PUTCHAR('\n'); }

    FORMAT_OPTIONS_TAB(f, 1);
    while(obj != NULL) {
        char * name = (char * ) ((list) obj->el)->el;
        list_element_pointer Inhalt = ((list) obj->el)->next;

        dest = object_sprint_format(dest, FORMAT_OPTIONS_SUM(f));
        // name
        OBJECT_JSON_STRINGIFY_PUTCHAR('\"'); OBJECT_JSON_STRINGIFY_PUTS(name); OBJECT_JSON_STRINGIFY_PUTCHAR('\"');
        // column
        if (f.allow_spaces) { OBJECT_JSON_STRINGIFY_PUTCHAR(' '); }
        OBJECT_JSON_STRINGIFY_PUTCHAR(':');
        if (f.allow_spaces) { OBJECT_JSON_STRINGIFY_PUTCHAR(' '); }

        // value
        if (Inhalt->type == Object) {
            object_JSON_stringify(dest, (object) Inhalt->el, f);
            OBJECT_JSON_STRINGIFY_RESETPOS();
        }
        else {
            if (Inhalt->type == String) { OBJECT_JSON_STRINGIFY_PUTCHAR('\"'); }
            list_element_sprint(Inhalt, dest, NULL); // this may be no good with lists - change in list library
            OBJECT_JSON_STRINGIFY_RESETPOS();
            if (Inhalt->type == String) { OBJECT_JSON_STRINGIFY_PUTCHAR('\"'); }
        }
        // end
        if (obj->next != NULL) { OBJECT_JSON_STRINGIFY_PUTCHAR(','); }
        if (Inhalt->type != List && Inhalt->type != List_pointer) {
            if (f.multi_line) { OBJECT_JSON_STRINGIFY_PUTCHAR('\n'); }
            else if (f.allow_spaces) { OBJECT_JSON_STRINGIFY_PUTCHAR(' '); }
        }
        // next element
        obj = obj->next;
    }
    FORMAT_OPTIONS_BACK(f, 1);
    dest = object_sprint_format(dest, FORMAT_OPTIONS_SUM(f));
    OBJECT_JSON_STRINGIFY_PUTCHAR('}');
}
ifndef PREFIX
	PREFIX := /usr/local
endif
ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif
DESTDIR := 

CC = gcc
AR = ar

AR_FLAGS = rcs #-c -r -v -s #crv rcs

LIB = 
INCLUDE = 

LIB_DIR = 
INCLUDE_DIR = 

BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

LIB_FILE = $(BUILD_DIR)/lib

ifdef lib
	LIB = $(lib)
endif
ifdef include
	INCLUDE = $(include)
endif

ifdef target
	ifeq ($(target), def)
		INCLUDE = templates/def.h
		INCLUDE_DIR = templates
	endif
	ifeq ($(target), simple_list)
		INCLUDE = templates/simple_list.h
		INCLUDE_DIR = templates
	endif
	ifeq ($(target), simple_array)
		INCLUDE = templates/simple_array.h
		INCLUDE_DIR = templates
	endif
	ifeq ($(target), words)
		INCLUDE = words_h.h
		LIB = build/libwords.a
	endif
	ifeq ($(target), list)
		INCLUDE = list_h.h
		LIB = build/liblist.a
	endif
endif

BUILD_DIR := build

install: install-header install-lib

ifneq ($(INCLUDE), )
install-header: $(INCLUDE)
	install -d $(DESTDIR)$(PREFIX)/include/Max2/$(INCLUDE_DIR)
	install -m 644 $(INCLUDE) $(DESTDIR)$(PREFIX)/include/Max2/$(INCLUDE)
endif

ifneq ($(LIB), )
install-lib: $(LIB)
	install -d $(DESTDIR)$(PREFIX)/lib/Max2/
	install -m 644 $(LIB) $(DESTDIR)$(PREFIX)/lib/Max2/
else
install-lib:
endif

.PNONY: install install-header install-lib # installing (see above)
.PNONY: words list # build libraries
.PHONY: clean # clean up

# build commands (extern)
words: $(LIB_FILE)words.a
list: $(LIB_FILE)list.a

# build any (intern)
$(LIB_FILE)%.a: build/obj/%.o
	$(AR) $(AR_FLAGS) $@ $^
	ranlib $@

$(OBJ_DIR)/%.o: src/%.h %_h.h
	$(CC) -c -fpic -x c -o $@ $<

# clean up
clean:
	rm -f build/obj/*.o
	rm -f build/*.a
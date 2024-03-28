ifndef PREFIX
	PREFIX := /usr/local
endif
ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif
DESTDIR := 

CC = gcc
AR = ar

CFLAGS = -c -fpic -x c
CFLAGS += -Wall -Wextra -Wno-missing-braces
CFLAGS += -O3
#CFLAGS += -g
AR_FLAGS = rcs #-c -r -v -s #crv rcs

LIB = 
INCLUDE = 

LIB_DIR = 
INCLUDE_DIR = 

BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj

LIB_FILE := $(BUILD_DIR)/lib

SRC_DIR := src

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
.PNONY: words word_pick list smartbuffer smartstring # build libraries
.PHONY: clean # clean up

# prepare commands

FOLDER =

smartbuffer: FOLDER = /smartbuffer
smartstring: FOLDER = /smartstring

SRC = $(wildcard $(SRC_DIR)$(FOLDER)/*.c)
OBJ = $(SRC:$(SRC_DIR)$(FOLDER)/%.c=$(OBJ_DIR)$(FOLDER)/%.o)

# build commands (extern)
words: $(LIB_FILE)words.a
word_pick: $(LIB_FILE)word_pick.a
list: $(LIB_FILE)list.a
smartbuffer: $(LIB_FILE)smartbuffer.a
smartstring: $(LIB_FILE)smartstring.a

# build any (intern)
$(LIB_FILE)%.a: $(OBJ_DIR)/%.o
	$(AR) $(AR_FLAGS) $@ $^
	ranlib $@

$(LIB_FILE)%.a: #$(OBJ)
	$(MAKE) $(OBJ)
	$(AR) $(AR_FLAGS) $@ $(OBJ)
	ranlib $@

$(OBJ_DIR)/smartbuffer/%.o: src/smartbuffer/%.c
	$(CC) $(CFLAGS) -o $@ $<

$(OBJ_DIR)/smartstring/%.o: src/smartstring/%.c
	$(CC) $(CFLAGS) -o $@ $<

$(OBJ_DIR)/%.o: src/%.h %_h.h
	$(CC) $(CFLAGS) -o $@ $<

# clean up
clean:
	rm -f $(OBJ_DIR)/*.o
	rm -f build/*.a
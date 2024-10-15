MAINDIR := ../../..
BUILDDIR := $(MAINDIR)/build
BINDIR := bin

CXX := g++
CXXFLAGS = -std=c++11 -Wall -Wextra

# for including source code directly
CXXFLAGS += -I../../../smart/buffer/c++/include/
CXXFLAGS += -I../../../smart/buffer/include/

# for macos
LDFLAGS = -L/opt/homebrew/lib
LDFLAGS += -L/usr/local/lib
LDFLAGS += -L$(BUILDDIR)

LDFLAGS += -isystem benchmark/include
LDFLAGS += -lbenchmark

LDFLAGS_NOLIB := $(LDFLAGS)

LDFLAGS += -lsmartbuffer -lc++smartbuffer -lsmartstring

TESTS := #\
	string_append_cont

#SRC = $(wildcard $(TESTS).cpp)
#DEST = $(TESTS)

LIBS_DEP := \
	$(BUILDDIR)/libwords.a     \
	$(BUILDDIR)/libword_pick.a

LIBS = $(LIBS_DEP) \
	$(BUILDDIR)/libsmartstring.a

#OPT=0

define build_lib = 
	$(BUILDDIR)/$(1).a:
		cd $(MAINDIR) && make $(1)
endef

$(BUILDDIR)/%.a:
	cd $(MAINDIR) && make $(@:$(BUILDDIR)/lib%.a=%)

ifdef TEST
SRC = $(TEST).cpp
DEST = $(TEST)
TESTNAME = $(TEST)
endif

ifeq ($(OPT), 1)
CXXFLAGS += -O3
endif
ifeq ($(DEBUG), 1)
CXXFLAGS += -g
endif

.PHONY: build nolib memtest norealloc run
.PHONY: autotest autotest_mem

# -L $(BUILDDIR) -lsmartbuffer -lsmartstring -lwords -lword_pick

build: $(LIBS)
	$(CXX) $(CXXFLAGS) -o $(BINDIR)/$(DEST) $(SRC) $(LDFLAGS) -lword_pick -lwords

nolib: LDFLAGS = $(LDFLAGS_NOLIB)
nolib: CXXFLAGS += -DINCLUDE_C_SOURCES=1
nolib: build

memtest: CXXFLAGS += -DMEMORY_TEST=1
memtest: nolib

norealloc: CXXFLAGS += -DNO_REALLOC
norealloc: nolib

usesizet: CXXFLAGS += -DUSE_SIZE_T=1
usesizet: nolib

run: $(BINDIR)/$(DEST)
	./$(BINDIR)/$(DEST) $(PARAMETERS)

# autotest section

TIMESTAMP := $(shell date -Iseconds)
OS_UNAME := $(shell uname)

AUTOTEST_RESULTS_DIR_BASE := results

AUTOTEST_RESULTS_DIR_OPT = $(AUTOTEST_RESULTS_DIR_BASE)/noopt
ifeq ($(OPT), 1)
	AUTOTEST_RESULTS_DIR_OPT = $(AUTOTEST_RESULTS_DIR_BASE)/opt
endif
ifeq ($(DEBUG), 1)
	AUTOTEST_RESULTS_DIR_OPT = $(AUTOTEST_RESULTS_DIR_BASE)/debug
endif

autotest_mem: TESTNAME = $(TEST)/mem

AUTOTEST_RESULTS_DIR = $(AUTOTEST_RESULTS_DIR_OPT)/$(OS_UNAME)/$(TESTNAME)/$(TIMESTAMP)

autotest_mkdir:
	mkdir -p $(AUTOTEST_RESULTS_DIR)

autotest_console: PARAMETERS = --benchmark_out=$(AUTOTEST_RESULTS_DIR)/console.txt --benchmark_out_format=console
autotest_json:    PARAMETERS = --benchmark_out=$(AUTOTEST_RESULTS_DIR)/result.json --benchmark_out_format=json
autotest_csv:     PARAMETERS = --benchmark_out=$(AUTOTEST_RESULTS_DIR)/result.csv  --benchmark_out_format=csv

autotest_console: run
autotest_json:    run
autotest_csv:     run

.PHONY: autotest_console autotest_json autotest_csv

autotest: autotest_mkdir
	$(MAKE) autotest_console TEST=$(TEST) OPT=$(OPT) AUTOTEST_RESULTS_DIR=$(AUTOTEST_RESULTS_DIR)
	$(MAKE) autotest_json    TEST=$(TEST) OPT=$(OPT) AUTOTEST_RESULTS_DIR=$(AUTOTEST_RESULTS_DIR)
#$(MAKE) autotest_csv     TEST=$(TEST) OPT=$(OPT) AUTOTEST_RESULTS_DIR=$(AUTOTEST_RESULTS_DIR)

autotest_mem: autotest_mkdir autotest_json
	tools/memtestJson.sh $(AUTOTEST_RESULTS_DIR)/result.json


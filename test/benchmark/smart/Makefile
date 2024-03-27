
CXX := g++
CXXFLAGS = -std=c++11 -Wall -Wextra

# for macos
LDFLAGS = -L/opt/homebrew/lib

LDFLAGS += -isystem benchmark/include
LDFLAGS += -lbenchmark

TESTS := \
	string_append_cont

MAINDIR := ../../..
BUILDDIR := $(MAINDIR)/build

SRC = $(wildcard $(TESTS).cpp)
DEST = $(TESTS)

LIBS = \
	$(BUILDDIR)/libsmartbuffer.a \
	$(BUILDDIR)/libsmartstring.a \
	$(BUILDDIR)/libwords.a       \
	$(BUILDDIR)/libword_pick.a

OPT=0

define build_lib = 
	$(BUILDDIR)/$(1).a:
		cd $(MAINDIR) && make $(1)
endef

$(BUILDDIR)/%.a:
	cd $(MAINDIR) && make $(@:$(BUILDDIR)/lib%.a=%)

ifdef test
	SRC = $(test).cpp
	DEST = $(test)
endif

.PHONY: opt build run autotest

# -L $(BUILDDIR) -lsmartbuffer -lsmartstring -lwords -lword_pick

build: $(LIBS)
	$(CXX) $(CXXFLAGS) -o $(DEST) $(SRC) $(LIBS) $(LDFLAGS)

opt: CXXFLAGS += -O3
opt: build

run: $(DEST)
	./$(DEST) $(PARAMETERS)

# autotest section

TIMESTAMP := $(shell date -Ihours)
OS_UNAME := $(shell uname)

AUTOTEST_RESULTS_DIR_BASE := results

AUTOTEST_RESULTS_DIR_OPT = $(AUTOTEST_RESULTS_DIR_BASE)/noopt
ifeq ($(OPT), 1)
	AUTOTEST_RESULTS_DIR_OPT = $(AUTOTEST_RESULTS_DIR_BASE)/opt
endif

AUTOTEST_RESULTS_DIR = $(AUTOTEST_RESULTS_DIR_OPT)/$(OS_UNAME)/$(TIMESTAMP)

autotest_mkdir:
	mkdir -p $(AUTOTEST_RESULTS_DIR)

autotest_console: PARAMETERS = --benchmark_out=$(AUTOTEST_RESULTS_DIR)/console.txt --benchmark_out_format=console
autotest_json:    PARAMETERS = --benchmark_out=$(AUTOTEST_RESULTS_DIR)/result.json --benchmark_out_format=json
autotest_csv:     PARAMETERS = --benchmark_out=$(AUTOTEST_RESULTS_DIR)/result.csv  --benchmark_out_format=json

.PHONY: autotest_console autotest_json autotest_csv

autotest: autotest_mkdir autotest_console run

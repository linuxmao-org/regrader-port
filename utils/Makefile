CXX ?= g++
CXXFLAGS ?= -O2 -g
LDFLAGS ?=

CXXFLAGS += -std=c++11
CXXFLAGS += -Wall -Wextra
CXXFLAGS += -MD -MP
CXXFLAGS += -Isources

TARGET_MACHINE := $(shell $(CXX) -dumpmachine)
ifneq (,$(findstring mingw,$(TARGET_MACHINE)))
APP_EXT := .exe
LDFLAGS += -static
endif

SOURCES := sources/rescc.cpp
OBJS := $(patsubst sources/%.cpp,build/%.o,$(SOURCES))

all: bin/rescc$(APP_EXT)

clean:
	rm -rf bin build

bin/rescc$(APP_EXT): $(OBJS)
	@mkdir -p bin
	$(CXX) -o $@ $^ $(LDFLAGS)

build/%.o: sources/%.cpp
	@mkdir -p build
	$(CXX) -c -o $@ $< $(CXXFLAGS)

.PHONY: all clean

-include $(OBJS:%.o=%.d)

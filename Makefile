CXX      ?= g++
CXXFLAGS ?= -O2 -std=c++17 -Wall -Wextra -pthread

HDRS = src/global_lock_list.hpp src/hoh_list.hpp

ifeq ($(OS),Windows_NT)
EXE = .exe
endif

all: test$(EXE) bench$(EXE)

test$(EXE): src/test.cpp $(HDRS)
	$(CXX) $(CXXFLAGS) -o $@ src/test.cpp

bench$(EXE): src/bench.cpp $(HDRS)
	$(CXX) $(CXXFLAGS) -o $@ src/bench.cpp

check: test$(EXE)
	./test$(EXE)

clean:
	rm -f test$(EXE) bench$(EXE)

.PHONY: all check clean

CXX      ?= g++
CXXFLAGS ?= -O2 -std=c++17 -Wall -Wextra -pthread

ifeq ($(OS),Windows_NT)
EXE = .exe
endif

all: test$(EXE)

test$(EXE): src/test.cpp src/global_lock_list.hpp
	$(CXX) $(CXXFLAGS) -o $@ src/test.cpp

check: test$(EXE)
	./test$(EXE)

clean:
	rm -f test$(EXE)

.PHONY: all check clean

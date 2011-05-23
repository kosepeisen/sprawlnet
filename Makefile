OBJS := $(patsubst %.cc,%.o,$(wildcard src/*.cc))
TESTOBJS := $(patsubst %.cc,%.o,$(wildcard tests/*.cc))

CXX=g++
CXXFLAGS=-Wall -pedantic -Isrc -g

LIBS=
TARGET=./main

TESTLIBS=-lgtest_main -lpthread
TESTTARGET=tests/runtests

EVTARGET=evaluator

.PHONY:all
all: main test

.PHONY:main
main: main.o $(OBJS)
	@echo "$(OBJS)"
	$(CXX) main.o $(OBJS) $(CXXFLAGS) -o $(TARGET) $(LIBS)

.PHONY:test
test: $(OBJS) $(TESTOBJS)
	$(CXX) $(TESTOBJS) $(OBJS) -o $(TESTTARGET) $(TESTLIBS)
	TERM=xterm $(TESTTARGET) --gtest_shuffle 2> /dev/null

.PHONY:memcheck
memcheck:
	valgrind --tool=memcheck --leak-check=yes $(TARGET)

.PHONY:clean
clean:
	rm -f */*.o *.o 

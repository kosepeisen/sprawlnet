OBJS := $(patsubst %.cc,%.o,$(wildcard src/*.cc src/*/*.cc))
TESTOBJS := $(patsubst %.cc,%.o,$(wildcard tests/*.cc))

CXX=g++
CXXFLAGS=-Wall -pedantic -Isrc -g

LIBS=`pkg-config --cflags --libs protobuf`
TARGET=./main

TESTLIBS=-lgtest -lgtest_main -lpthread
TESTTARGET=tests/runtests

PROTOC=protoc
PROTOFLAGS=-Iproto --cpp_out=src/message

.PHONY:all
all: main test

.PHONY:proto
proto:
	mkdir -p src/message
	$(PROTOC) $(PROTOFLAGS) proto/message.proto

.PHONY:main
main: proto main.o $(OBJS) 
	@echo "$(OBJS)"
	$(CXX) main.o $(OBJS) $(CXXFLAGS) -o $(TARGET) $(LIBS)

.PHONY:test
test: $(OBJS) $(TESTOBJS)
	$(CXX) $(TESTOBJS) $(OBJS) -o $(TESTTARGET) $(TESTLIBS) $(LIBS)

.PHONY:runtest
runtest: test
	TERM=xterm $(TESTTARGET) --gtest_shuffle 2> /dev/null

.PHONY:memcheck
memcheck:
	valgrind --tool=memcheck --leak-check=yes $(TARGET)

.PHONY:clean
clean:
	rm -f src/message/*.pb.{cc,h,o}
	rm -f */*.o *.o 

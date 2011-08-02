CXX = g++

CPPFLAGS += -I../src
CXXFLAGS += -Wall -Wextra -g `pkg-config --cflags protobuf`

LDLIBS += ../libsprawl.a -lgtest -lgtest_main -lpthread `pkg-config --libs protobuf`

OBJS := $(patsubst %.cc,%.o,$(wildcard *.cc))
TARGET = runtest

.PHONY: all
all: runtest

$(TARGET): $(OBJS)
	$(CXX) $^ -o $@ $(LDLIBS)

.PHONY: run
run: $(TARGET)
	TERM=xterm ./$(TARGET) --gtest_shuffle 2> /dev/null

.PHONY: clean
clean:
	rm -f $(OBJS)

.PHONY: purge
purge: clean
	rm -f $(TARGET)

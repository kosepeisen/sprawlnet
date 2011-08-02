AR = ar
CXX = g++
PROTOC = protoc

ARFLAGS ?= -rv

CPPFLAGS += -Isrc
CXXFLAGS +=-Wall -Wextra -g `pkg-config --cflags protobuf`

LDLIBS +=`pkg-config --libs protobuf` $(LIB_TARGET)

PROTOFLAGS +=-Iproto --cpp_out=src/message

PROTO_OBJS := src/message/message_header.pb.o
PROTO_HEADERS = $(patsubst %.o,%.h,$(PROTO_OBJS))

LIB_OBJS := $(PROTO_OBJS) $(patsubst %.cc,%.o,$(wildcard src/*.cc))
LIB_DEPENDS = $(patsubst %.o,%.d,$(LIB_OBJS))
LIB_TARGET = libsprawl.a

SERVER_OBJS = server.o
SERVER_DEPENDS = $(patsubst %.o,%.d,$(SERVER_OBJS))
SERVER_TARGET = server

CLIENT_OBJS = client.o
CLIENT_DEPENDS = $(patsubst %.o,%.d,$(CLIENT_OBJS))
CLIENT_TARGET = client

.PHONY: all
all: $(LIB_TARGET) $(SERVER_TARGET) $(CLIENT_TARGET)

ifneq ($(MAKECMDGOALS), clean)
ifneq ($(MAKECMDGOALS), purge)
-include $(LIB_DEPENDS)
endif
endif

$(LIB_DEPENDS): src/message $(PROTO_HEADERS)

$(LIB_TARGET): $(LIB_OBJS)
	$(AR) $(ARFLAGS) $@ $(LIB_OBJS)

src/message:
	mkdir -p src/message

src/message/%.pb.h src/message/%.pb.cc: proto/%.proto
	$(PROTOC) $(PROTOFLAGS) $<

.PHONY: test
test: $(LIB_TARGET)
	$(MAKE) run -C tests

.PHONY: clean
clean:
	rm -f src/message/*.pb.cc src/message/*.pb.h
	rm -f $(LIB_DEPENDS) $(SERVER_DEPENDS) $(CLIENT_DEPENDS)
	rm -f $(LIB_OBJS) $(SERVER_OBJS) $(CLIENT_OBJS)
	$(MAKE) clean -C tests

.PHONY: purge
purge: clean
	rm -f $(LIB_TARGET) $(SERVER_TARGET) $(CLIENT_TARGET)
	$(MAKE) purge -C tests

.PHONY: memcheck
memcheck: $(LIB_TARGET) $(SERVER_TARGET)
	valgrind --tool=memcheck --leak-check=yes ./$(SERVER_TARGET)

%.d: %.cc
	@echo Collecting dependency information for $<
	@$(SHELL) -ec '$(CXX) -MM $(CPPFLAGS) $< \
		| sed '\''s,\($(*F)\)\.o[ :]*,$*.o $@ : ,g'\'' > $@'

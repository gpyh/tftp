CC = gcc
CFLAGS = -std=c11 -Wall -pthread -Wextra -Werror -pedantic -D_XOPEN_SOURCE=700 -Iinclude/ -IAdrInet/include -Isudp/include
LDFLAGS = -pthread
LDLIBS = 

.PHONY: all clean test

all: bin/tftp

clean:
	$(RM) -f bin/* obj/* tests/bin/* tests/obj/*
	$(MAKE) -C AdrInet clean
	$(MAKE) -C sudp clean

test: tests/bin/common
	-tests/bin/common


bin/tftp: $(patsubst %,obj/%.o,tftp serve fetch send common debug backlog) AdrInet/lib/libAdrInet.a sudp/lib/libsudp.a

bin/%: obj/%.o
	$(CC) $(LDFLAGS) -o $@ $^

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<


tests/bin/common: tests/obj/common.o obj/common.o AdrInet/lib/libAdrInet.a sudp/lib/libsudp.a

tests/bin/%: tests/obj/%.o
	$(CC) $(LDFLAGS) -o $@ $^

tests/obj/%.o: tests/src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

AdrInet/lib/libAdrInet.a:
	$(MAKE) -C AdrInet all

sudp/lib/libsudp.a:
	$(MAKE) -C sudp all


CC = gcc
CFLAGS = -std=c11 -Wall -pthread -Wextra -Werror -pedantic -D_XOPEN_SOURCE=700 -Iinclude/ -IAdresseInternet/include -ISocketUDP/include
LDFLAGS =
LDLIBS = 

.PHONY: all clean test

all: bin/tftp

clean:
	$(RM) -f bin/* obj/* tests/bin/* tests/obj/*
	$(MAKE) -C AdresseInternet clean
	$(MAKE) -C SocketUDP clean

test: tests/bin/common
	-tests/bin/common


bin/tftp: $(patsubst %,obj/%.o,tftp serve fetch send)

obj/tftp.o: src/tftp.c $(patsubst %,include/%.h,serve fetch send)

obj/serve.o: src/serve.c

obj/fetch.o: src/fetch.c

obj/send.o: src/send.c

obj/common.o: src/common.c include/common.h

bin/%: obj/%.o
	$(CC) $(LDFLAGS) -o $@ $^

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<


tests/bin/common: tests/obj/common.o obj/common.o AdresseInternet/lib/libAdresseInternet.a SocketUDP/lib/libSocketUDP.a

tests/bin/%: tests/obj/%.o
	$(CC) $(LDFLAGS) -o $@ $^

tests/obj/%.o: tests/src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

AdresseInternet/lib/libAdresseInternet.a:
	$(MAKE) -C AdresseInternet all

SocketUDP/lib/libSocketUDP.a:
	$(MAKE) -C SocketUDP all


CC = gcc
CFLAGS = -std=c11 -Wall -pthread -Wextra -Werror -pedantic -D_XOPEN_SOURCE=700 -Iinclude/ -fPIC
LDFLAGS =
LDLIBS = 

.PHONY: all clean test

all: lib/libAdrInet.a lib/libAdrInet.so

clean:
	$(RM) -f obj/* tests/bin/* tests/obj/* 

test: TestAdrInet.c all
	gcc $(CFLAGS) -Wl,--rpath=../lib/ $<   -o test $(LDFLAGS)


lib/libAdrInet.a: obj/AdrInet.o
	ar r $@ $<

lib/libAdrInet.so: obj/AdrInet.o
	gcc -shared $< -o $@

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $< 


tests/bin/AdrInet: tests/obj/AdrInet.o obj/AdrInet.o

tests/bin/%: tests/obj/%.o
	$(CC) $(LDFLAGS) -o $@ $^

tests/obj/%.o: tests/src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<


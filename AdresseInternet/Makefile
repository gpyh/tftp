CC = gcc
CFLAGS = -std=c11 -Wall -pthread -Wextra -Werror -pedantic -D_XOPEN_SOURCE=700 -Iinclude/ -fPIC
LDFLAGS =
LDLIBS = 

.PHONY: all clean test

all: lib/libAdresseInternet.a lib/libAdresseInternet.so

clean:
	$(RM) -f obj/* tests/bin/* tests/obj/* 

test: TestAdresseInternet.c all
	gcc $(CFLAGS) -Wl,--rpath=../lib/ $<   -o test $(LDFLAGS)


lib/libAdresseInternet.a: obj/AdresseInternet.o
	ar r $@ $<

lib/libAdresseInternet.so: obj/AdresseInternet.o
	gcc -shared $< -o $@

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $< 


tests/bin/AdresseInternet: tests/obj/AdresseInternet.o obj/AdresseInternet.o

tests/bin/%: tests/obj/%.o
	$(CC) $(LDFLAGS) -o $@ $^

tests/obj/%.o: tests/src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<


CC = gcc
CFLAGS = -std=c11 -Wall -pthread -Wextra -Werror -pedantic -D_XOPEN_SOURCE=700 -Iinclude/ -I../AdresseInternet/include -fPIC
LDFLAGS =
LDLIBS = 

.PHONY: all clean test

all: lib/libsudp.a lib/libsudp.so

clean:
	$(RM) -f obj/* tests/bin/* tests/obj/* 

test: tests/bin/sudp
	-tests/bin/sudp


lib/libsudp.a: obj/sudp.o
	ar r $@ $<

lib/libsudp.so: obj/sudp.o
	gcc -shared $< -o $@

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $< 


tests/bin/sudp: tests/obj/sudp.o obj/sudp.o ../AdresseInternet/lib/libAdresseInternet.a

tests/bin/%: tests/obj/%.o
	$(CC) $(LDFLAGS) -o $@ $^

tests/obj/%.o: tests/src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

../AdresseInternet/lib/libAdresseInternet.a:
	$(MAKE) -C ../AdresseInternet all


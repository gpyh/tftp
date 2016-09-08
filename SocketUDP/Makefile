CC = gcc
CFLAGS = -std=c11 -Wall -pthread -Wextra -Werror -pedantic -D_XOPEN_SOURCE=700 -Iinclude/ -I../AdresseInternet/include -fPIC
LDFLAGS =
LDLIBS = 

.PHONY: all clean test

all: lib/libSocketUDP.a lib/libSocketUDP.so

clean:
	$(RM) -f obj/* tests/bin/* tests/obj/* 

test: tests/bin/SocketUDP
	-tests/bin/SocketUDP


lib/libSocketUDP.a: obj/SocketUDP.o
	ar r $@ $<

lib/libSocketUDP.so: obj/SocketUDP.o
	gcc -shared $< -o $@

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $< 


tests/bin/SocketUDP: tests/obj/SocketUDP.o obj/SocketUDP.o ../AdresseInternet/lib/libAdresseInternet.a

tests/bin/%: tests/obj/%.o
	$(CC) $(LDFLAGS) -o $@ $^

tests/obj/%.o: tests/src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

../AdresseInternet/lib/libAdresseInternet.a:
	$(MAKE) -C ../AdresseInternet all


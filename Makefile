CC = gcc
CFLAGS = -std=c11 -Wall -pthread -Wextra -Werror -pedantic -D_XOPEN_SOURCE=600 -Iinclude/
LDFLAGS =
LDLIBS = 
SRC = $(wildcard *.c)
OBJ = $(SOURCES:.c=.o)

.PHONY: all clean

all: build/tftp

test: build/tests/common
	-./build/tests/common

clean:
	find build -type f -delete

build/tftp: $(patsubst %,build/%,tftp.o serve.o fetch.o send.o)

build/tftp.o: tftp.c $(patsubst %,include/%,serve.h fetch.h send.h)

build/serve.o: serve.c

build/fetch.o: fetch.c

build/send.o: send.c

build/common.o: common.c include/common.h

build/tests/common: build/tests/common.o build/common.o

build/tests/common.o: tests/common.c include/common.h

build/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<


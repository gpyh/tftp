CC = gcc
CFLAGS = -std=c11 -Wall -pthread -Wextra -Werror -pedantic -D_XOPEN_SOURCE=600
LDFLAGS =
LDLIBS = 
SRC = $(wildcard *.c)
OBJ = $(SOURCES:.c=.o)
BIN = tftp

all: $(BIN)

tftp: tftp.o serve.o fetch.o send.o

tftp.o: tftp.c serve.h fetch.h send.h

serve.o: serve.c

fetch.o: fetch.c

send.o: send.c

clean:
	$(RM) -f *.o $(BIN)

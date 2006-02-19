SHELL = /bin/sh

# Add any source file names here
# TestExample can be deleted
FILES = tester TestExample

BIN = AdminServiceTest
SRC = $(addsuffix .cpp, $(FILES))
OBJ = $(addsuffix .o, $(FILES))

CC = g++
CFLAGS = -c
LDFLAGS = -O -I/usr/local/include -lcppunit -ldl

INSTALLPATH = ../Debug/linux

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

$(BIN): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

all: $(BIN)

clean: 
	rm -f $(BIN) *.o core

install: $(BIN)
	mv -f $(BIN) $(OBJ) $(INSTALLPATH)

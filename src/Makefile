#Bryan MArtin Tostado
#creation: april 27ish 2014
#Makefile for compiler project sla (symbolic language analyzer)

CC=gcc
CFLAGS=-c -std=c99
LDFLAGS=
SOURCES= $(wildcard *.c) 
OBJECTS= $(SOURCES:.c=.o)
EXECUTABLE= sla 

all: build

.PHONY: debug
debug: CFLAGS+= -g
debug: clean build

.PHONY: build
build: $(OBJECTS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -f *.o $(EXECUTABLE) 

.PHONY: help
help:
	@echo " available commands:"
	@echo "    build : builds sla"
	@echo "    debug : builds sla with debug symbols"
	@echo "    clean "

# Makefile for CS 2110 Homework 11, Fall 2018

CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -Werror -Wextra -g
CHECK_LIBS = $(shell pkg-config --cflags --libs check)

# The C and H files
CFILES = my_malloc.c tests.c $(patsubst %.c,%.o,$(wildcard suites/*.c))
HFILES = my_malloc.h my_sbrk.h suites/suites.h
OFILES = $(patsubst %.c,%.o,$(CFILES))

.PHONY: run-tests run-gdb clean

tests: $(OFILES)
	./verify.sh
	$(CC) $(CFLAGS) $^ -o $@ $(CHECK_LIBS)

%.o: %.c $(HFILES)
	$(CC) $(CFLAGS) -c $< -o $@

run-tests: tests
	mkdir -p actual
	mkdir -p diff
	./tests $(TEST)
	./difference.sh

run-gdb: tests
	CK_FORK=no gdb --args ./tests $(TEST)

clean:
	rm -rf actual
	rm -rf diff
	rm -rf tests $(OFILES)

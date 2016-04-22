EMACS_ROOT ?= ../..
EMACS ?= emacs

CC      = gcc
LD      = gcc
CPPFLAGS = -I$(EMACS_ROOT)/src
CFLAGS = -std=gnu99 -ggdb3 -Wall -fPIC $(CPPFLAGS)

.PHONY : test

all: mecab-core.so

mecab-core.so: mecab-core.o
	$(LD) -shared $(LDFLAGS) -o $@ $^ -lmecab

mecab-core.o: mecab-core.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-rm -f mecab-core.so mecab-core.o

test:
	$(EMACS) -Q -batch -L . $(LOADPATH) \
		-l test/test.el \
		-f ert-run-tests-batch-and-exit

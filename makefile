.PHONY: clean ll-test

DEFAULT: test
# Define the tools we are going to use
CC= g++
LD = g++

LIBS = -lm -lpthread -lrt -latomic
CFLAGS = -g -Wall -Wno-unused -Wno-return-type -Wno-sign-compare -std=c++17 -I.

JUNK = *~ *.o *.a *_ut  *-test *-*

bst-test: bst_test.o bst.o
	$(LD) $(CFLAGS) -o $@ $^ $(LIBS) 

bst_test.o: bst_test.cc
	$(CC) $(CFLAGS) -c -o $@ $<

bst.o: bst.cc
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f  test $(JUNK) 

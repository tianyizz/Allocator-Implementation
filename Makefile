CC=gcc
CFLAGS=-Wall -g
FASTFLAGS = -DNDEBUG -Wall -fPIC -shared


BINS=  libmyalloc.so


all: $(BINS)


libmyalloc.so: allocator.c
	$(CC) $(FASTFLAGS) -o libmyalloc.so allocator.c -ldl

clean:
	rm $(BINS)

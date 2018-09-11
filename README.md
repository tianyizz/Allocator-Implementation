# Goal
The program is to implement malloc, realloc, free, calloc using mmap and munmap. The program should do anything that the functions provided by system did.

# Implementation
Make
LD_PRELOAD=libmyalloc.so
./program_you_want_to_test

bin_PROGRAMS=test
test_SOURCES=test.c sstring.c

test_CFLAGS = -g -I./include

lib_LIBRARIES=libclib.a
libclib_a_SOURCES=\
	core.c \
	sstring.c \
	slist.c \
	hash_table.c \
	bitmap.c

 
libclib_a_CFLAGS = -Iinclude
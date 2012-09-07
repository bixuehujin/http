/*
 * sstring.c
 *
 *  Created on: 2012-9-7
 *      Author: hujin
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "helper.h"
#include "sstring.h"

sstring_t * sstring_new(size_t size) {
	sstring_t * ss = m_new(sstring_t, 1);
	assert(ss != NULL);
	ss->len = 0;
	ss->_alloc = size;

	ss->ptr = malloc(size);
	assert(ss->ptr != NULL);

	return ss;
}

sstring_t * sstring_new_stack(size_t size) {
	sstring_t * ss = alloca(sizeof(sstring_t));
	assert(ss != NULL);
	ss->len = 0;
	ss->_alloc = size;

	ss->ptr = alloca(size);
	assert(ss->ptr != NULL);

	return ss;
}


void sstring_init(sstring_t * ss, size_t size) {
	(*ss).len = 0;
	(*ss)._alloc = size;

	(*ss).ptr = malloc(size);
	assert((*ss).ptr != NULL);
}


void sstring_init_stack(sstring_t * ss, size_t size) {
	(*ss).len = 0;
	(*ss)._alloc = size;

	(*ss).ptr = alloca(size);
	assert((*ss).ptr != NULL);
}


void sstring_destroy(sstring_t *ss) {
	assert(ss);
	assert(ss->ptr);
	free(ss->ptr);
}


void sstring_free(sstring_t * ss) {
	assert(ss);
	assert(ss->ptr != NULL);
	free(ss->ptr);
	free(ss);
}


bool sstring_append(sstring_t * ss, const char * str) {
	size_t len = strlen(str);
	size_t ss_len = ss->len;
	char * ptr = ss->ptr;

	if(ss->_alloc < ss_len + len + 1) {
		int c = (len + ss_len) / ss->_alloc + 1;
		char * nptr = NULL;

		ss->_alloc *= c;
		nptr = realloc(ss->ptr, ss->_alloc);
		if(!nptr) return false;

		ss->ptr = nptr;
	}
	strcpy(ptr + ss->len, str);
	ss->len += len;
	return true;
}


bool sstring_sprintf_append(sstring_t * ss, const char * format, ...) {
	int n;
	size_t ss_len = ss->len;
	int buffsize = ss->_alloc - ss_len;

	va_list va;
	va_start(va, format);
	n = vsnprintf(ss->ptr + ss_len, buffsize, format, va);
	va_end(va);

	if(n < 0) return false;

	if(n >= buffsize) { //memory truncated , do realloc.
		int c = (n + ss_len) / ss->_alloc + 1;
		char * nptr = NULL;

		ss->_alloc *= c;
		nptr = realloc(ss->ptr, ss->_alloc);
		if(!nptr) return false;

		ss->ptr = nptr;
	}
	buffsize = ss->_alloc - ss_len;

	va_start(va, format);
	n = vsnprintf(ss->ptr + ss_len, buffsize, format, va);
	if(n < 0) return false;
	va_end(va);

	ss->len += n;
	return true;
}



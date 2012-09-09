/*
 * sstring.h
 *
 *  Created on: 2012-9-6
 *      Author: hujin
 */

#ifndef SSTRING_H_
#define SSTRING_H_
#include <stdio.h>
#include "helper.h"

typedef struct _sstring{
	size_t len;
	size_t _alloc;
	char _use_stack;
	char * ptr;
}sstring_t;

#define sstring_empty(pss) \
	(((pss)->ptr == NULL || (pss)->len == 0))

#define sstring_for_init \
	{0, 0, 0, NULL}


sstring_t * sstring_new(size_t size);
void string_init(sstring_t * ss, size_t size);
sstring_t * sstring_new_stack(size_t size);
void sstring_init_stack(sstring_t * ss, size_t size);
bool sstring_append(sstring_t * ss, const char * str);
bool sstring_sprintf_append(sstring_t * ss, const char * format, ...);
void string_destroy(sstring_t * ss);
void sstring_free(sstring_t * ss);

#endif /* SSTRING_H_ */

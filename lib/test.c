/*
 * test.c
 *
 *  Created on: 2012-9-7
 *      Author: hujin
 */



#include "clib.h"


int main(int argc, char **argv) {


	//sstring_t * sss = m_new(sstring_t, 1);

	sstring_t * ss = sstring_new(5);
	sstring_append(ss, "foo");
	sstring_append(ss, "ara");
	printf("out:%s - %ld\n", ss->ptr, ss->_alloc);

	sstring_sprintf_append(ss, "format%s:%d\n", "ff", 1000);
	printf("out:%s - %ld\n", ss->ptr, ss->_alloc);
	printf("len1:%ld len2:%ld\n", ss->len, strlen(ss->ptr));
	sstring_free(ss);

	return 0;
}


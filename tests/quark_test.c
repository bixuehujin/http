/*
 * quark_test.c
 *
 *  Created on: Sep 10, 2012
 *      Author: hujin
 */

#include <stdio.h>
#include <clib.h>


int main(int argc, char **argv) {

	clib_init();

	quark_t q1 = quark_form_string("quark string 1");
	quark_t q2 = quark_form_string("quark string 2");
	quark_t q3 = quark_form_string("quark string 2");
	printf("q1: %s %d\n", quark_get_string(q1), q1);
	quark_delete(q2);
	q3 = quark_form_string("quark string 2");
	printf("q2: %s %d\n", quark_get_string(q2), q2);
	printf("q3: %s %d\n", quark_get_string(q3), q3);


	return 0;
}



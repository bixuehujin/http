/*
 * core.c
 *
 *  Created on: Sep 10, 2012
 *      Author: hujin
 */

#include "clib/helper.h"
#include "clib/hash_table.h"

bool clib_global_inited = false;


void clib_init() {
	clib_global_inited = true;
}


bool clib_is_inited() {
	return clib_global_inited;
}



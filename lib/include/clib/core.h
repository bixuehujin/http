/*
 * core.h
 *
 *  Created on: Sep 10, 2012
 *      Author: hujin
 */

#ifndef CORE_H_
#define CORE_H_

#include "clib/helper.h"
#include "clib/hash_table.h"

extern bool clib_global_inited;
extern hash_table_t * clib_global_quark;

void clib_init();

#endif /* CORE_H_ */

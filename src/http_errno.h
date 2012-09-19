/*
 * errno.h
 *
 *  Created on: Sep 19, 2012
 *      Author: hujin
 */

#ifndef ERRNO_H_
#define ERRNO_H_
#include <stdio.h>

enum {
	ERR_NONE,
	ERR_UNKNOWN,
	ERR_NET_UNREACH,
	ERR_CANNOT_RESOLVE_HOST,
	ERR_CREATE_SOCKET_FAILED,
	ERR_CONN_REFUSED,
	ERR_CONN_TIMEOUT,
};

static char * err_names[] = {
	"ERR_NONE",
	"ERR_UNKNOWN",
	"ERR_NET_UNREACH",
	"ERR_CANNOT_RESOLVE_HOST",
	"ERR_CREATE_SOCKET_FAILED",
	"ERR_CONN_REFUSED",
	"ERR_CONN_TIMEOUT",
	NULL
};


#endif /* ERRNO_H_ */

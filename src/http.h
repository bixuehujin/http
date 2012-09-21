/*
 * http.h
 *
 *  Created on: Sep 21, 2012
 *      Author: hujin
 */

#ifndef HTTP_H_
#define HTTP_H_

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <clib.h>

#define METHOD_NONE			0
#define METHOD_HEAD 		1
#define METHOD_GET 			2
#define METHOD_POST 		3
#define METHOD_PUT 			4
#define METHOD_DELETE 		5
#define METHOD_TRACE		6
#define METHOD_OPTIONS		7

static char * method_names[] = {
	"NONE",
	"HEAD",
	"GET",
	"POST",
	"PUT",
	"DELETE",
	"TRACE",
	"OPTIONS",
	NULL
};

typedef signed char http_method_t;

#endif /* HTTP_H_ */

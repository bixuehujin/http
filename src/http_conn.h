/*
 * http_conn.h
 *
 *  Created on: 2012-9-6
 *      Author: hujin
 */

#ifndef HTTP_CONN_H_
#define HTTP_CONN_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "helper.h"

typedef struct _http_conn {
	int connfd;
	char * host;
	short port;
}http_conn_t;


http_conn_t * http_conn_new(const char * host, unsigned short port);
bool http_conn_connect(http_conn_t * conn);
void http_conn_free(http_conn_t * conn);

#endif /* HTTP_CONN_H_ */

/*
 * http_conn.h
 *
 *  Created on: 2012-9-6
 *      Author: hujin
 */

#ifndef HTTP_CONN_H_
#define HTTP_CONN_H_

#include "http.h"


typedef struct _http_conn {
	int connfd;
	char * host;
	short port;
	cerror_t * error;
}http_conn_t;


http_conn_t * http_conn_new(const char * host, unsigned short port);
bool http_conn_connect(http_conn_t * conn);
void http_conn_free(http_conn_t * conn);

void http_conn_pool_start();
void http_conn_pool_shutdown();
http_conn_t * http_conn_pool_get_conn(const char * host, uint16_t port, cerror_t **error);

#endif /* HTTP_CONN_H_ */

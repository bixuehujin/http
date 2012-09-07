/*
 * http_conn.c
 *
 *  Created on: 2012-9-6
 *      Author: hujin
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "helper.h"
#include "http_conn.h"


http_conn_t * http_conn_new(const char * host, unsigned short port) {

	http_conn_t * conn = m_new0(http_conn_t, 1);
	assert(conn != NULL);

	return conn;
}


void http_conn_free(http_conn_t * conn) {
	assert(conn != NULL);
	free(conn);
}

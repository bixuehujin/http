/*
 * http_conn.c
 *
 *  Created on: 2012-9-6
 *      Author: hujin
 */

#include "http.h"
#include "http_conn.h"
#include "url.h"
#include "tcp.h"


http_conn_t * http_conn_new(const char * host, uint16_t port) {

	http_conn_t * conn = m_new0(http_conn_t, 1);
	assert(conn != NULL);

	conn->host = strdup(host);
	conn->port = port;

	conn->error = NULL;
	return conn;
}


bool http_conn_connect(http_conn_t * conn) {
	cerror_t * error = NULL;
	int fd = tcp_connect(conn->host, conn->port, &error);
	if(fd < 0) {
		conn->error = error;
		return false;
	}

	/*
	struct sockaddr_in sockaddr;
	memset(&sockaddr, 0, sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(conn->port);
	inet_pton(AF_INET, conn->host, &sockaddr.sin_addr);

	int r = connect(conn->connfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
	if(r < 0) {
		perror("connect()");
		return false;
	}
	*/
	conn->connfd = fd;
	return true;
}



void http_conn_free(http_conn_t * conn) {
	assert(conn != NULL);
	close(conn->connfd);
	free(conn->host);
	free(conn);
}


/*********************************/
/*   connection pools support    */
/*********************************/

static hash_table_t *conn_pool = NULL;

void http_conn_pool_start() {
	conn_pool = hash_table_new(32, http_conn_free);
}


http_conn_t * http_conn_pool_get_conn(const char * host, uint16_t port, cerror_t **error) {
	sstring_t stridx = sstring_for_init;
	sstring_fappend(&stridx, "%s:%d", host, port);
	http_conn_t * conn = hash_table_find(conn_pool, stridx.ptr);
	if(conn) {
		sstring_destroy(&stridx);
		return conn;
	}

	conn = http_conn_new(host, port);
	if(!http_conn_connect(conn)) {
		*error = conn->error;
		sstring_destroy(&stridx);
		return NULL;
	}

	hash_table_insert(conn_pool, stridx.ptr, conn);
	sstring_destroy(&stridx);
	return conn;
}


void http_conn_pool_shutdown() {
	hash_table_free(conn_pool);
}

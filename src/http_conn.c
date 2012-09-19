/*
 * http_conn.c
 *
 *  Created on: 2012-9-6
 *      Author: hujin
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "http_conn.h"
#include "url.h"
#include "tcp.h"

http_conn_t * http_conn_new(const char * host, uint16_t port) {

	http_conn_t * conn = m_new0(http_conn_t, 1);
	assert(conn != NULL);

	conn->host = strdup(host);
	conn->port = port;

	return conn;
}


bool http_conn_connect(http_conn_t * conn) {
	char * msg = NULL;
	int fd = tcp_connect(conn->host, conn->port, &msg);
	if(fd < 0) {
		conn->errno = ERR_CONN_CANNOT_RESOLVE_HOST;
		conn->errer = msg;
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
	if(conn->errer) {
		free(conn->errer);
	}
	free(conn);
}

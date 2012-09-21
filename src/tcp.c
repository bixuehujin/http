/*
 * tcp.c
 *
 *  Created on: Sep 19, 2012
 *      Author: hujin
 */

#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <clib.h>
#include <errno.h>
#include "http_errno.h"


static bool resolve_host(const char * host, uint16_t port, void * sockaddr, socklen_t *socklen, cerror_t ** error) {
	struct hostent *ent;

	if(!(ent = gethostbyname(host))) {
		if(error) {
			*error = cerror_new("resolve_host", ERR_CANNOT_RESOLVE_HOST, "%d: %s", h_errno, hstrerror(h_errno));
		}
		return false;
	}
	if(ent->h_addrtype != AF_INET) {
		return false;
	}

	struct sockaddr_in *in = (struct sockaddr_in * )sockaddr;
	in->sin_family = AF_INET;
	in->sin_port = htons(port);
	if(!ent->h_addr_list[0]) return false;

	memcpy(&in->sin_addr, ent->h_addr_list[0], sizeof(struct in_addr));
	*socklen = sizeof(struct sockaddr_in);

	return true;
}


/**
 * this function would block until the connection is established or an error occurred.
 */
int tcp_connect(const char * host, uint16_t port, cerror_t ** error) {
	int fd;
	struct sockaddr_in in;
	socklen_t socklen;
	cerror_t * oerror = NULL;

	if(!resolve_host(host, port, &in, &socklen, &oerror)) {
		*error = oerror;
		return -1;
	}


	fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd < 0) {
		*error = cerror_new("tcp_connect", ERR_CREATE_SOCKET_FAILED, "%d: %s", errno, strerror(errno));
		return -1;
	}

	if(connect(fd, (struct sockaddr *)&in, socklen) < 0) {
		fd_set rset, wset;
		struct timeval tmout;

		switch(errno) {
		case ECONNREFUSED:
			*error = cerror_new("tcp_connect", ERR_CONN_REFUSED, "%d: %s", errno, strerror(errno));;
			return -1;
		case EINTR:
			/**
			 * the connect system call may be interrupted by a signal, but the connection is continue,
			 * we should not restart the connect, but using select to wait the socket readable.
			 * @see UNPV1 chapter 16.4
			 */
			FD_ZERO(&rset);
			FD_ZERO(&wset);
			FD_SET(fd, &rset);
			FD_SET(fd, &wset);
			tmout.tv_sec = 10;
			tmout.tv_usec = 0;

			int r;
			for(;;) {
				r = select(fd + 1, &rset, &wset, NULL, &tmout);
				if(r < 0) {
					if(errno == EINTR) {
						continue;
					}
				}else if(r == 0) {
					*error = cerror_new("tcp_connect", ERR_CONN_TIMEOUT, strdup("Connection Timed Out"));
					return -1;
				}else {
					if(FD_ISSET(fd, &rset) && FD_ISSET(fd, &wset)) {//error occurred
						int nerror;
						socklen_t len = sizeof(nerror);
						if(getsockopt(fd, SOL_SOCKET, SO_ERROR, &nerror, &len) >= 0) {
							*error = cerror_new("tcp_connect", ERR_UNKNOWN, "%d: %s", errno, strerror(errno));
						}
						return -1;
					}
					return fd;
				}
			}
			break;
		case ETIMEDOUT:
			*error = cerror_new("tcp_connect", ERR_CONN_TIMEOUT, "%d: %s", errno, strerror(errno));
			return -1;
		default:
			*error = cerror_new("tcp_connect", ERR_UNKNOWN, "%d: %s", errno, strerror(errno));
			return -1;
		}
	}

	return fd;
}





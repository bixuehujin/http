/*
 * tcp.c
 *
 *  Created on: Sep 19, 2012
 *      Author: hujin
 */

#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <clib.h>
#include <errno.h>
#include "http_errno.h"


static bool resove_host(const char * host, uint16_t port, void * sockaddr, socklen_t *socklen, cerror_t ** error) {
	struct hostent *ent;
	sstring_t ss = sstring_for_init;

	if(!(ent = gethostbyname(host))) {
		if(error) {
			sstring_fappend(&ss, "%d: %s", h_errno, hstrerror(h_errno));
			*error = cerror_new("resolve_host", ERR_CANNOT_RESOLVE_HOST, ss.ptr);
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


int tcp_connect(const char * host, uint16_t port, cerror_t ** error) {
	int fd;
	struct sockaddr_in in;
	socklen_t socklen;
	sstring_t ss = sstring_for_init;
	cerror_t * oerror = NULL;

	if(!resove_host(host, port, &in, &socklen, &oerror)) {
		*error = oerror;
		return -1;
	}


	fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd < 0) {
		sstring_fappend(&ss, "%d: %s", errno, strerror(errno));
		*error = cerror_new("tcp_connect", ERR_CREATE_SOCKET_FAILED, ss.ptr);
		return -1;
	}

	again:
	if(connect(fd, (struct sockaddr *)&in, socklen) < 0) {
		sstring_fappend(&ss, "%d: %s", errno, strerror(errno));
		*error = cerror_new("tcp_connect", 0, NULL);;
		switch(errno) {
		case ECONNREFUSED:
			cerror_set_error(*error, ERR_CONN_REFUSED, ss.ptr);
			break;
		case EINTR:
			goto again;
			break;
		default:
			cerror_set_error(*error, ERR_UNKNOWN, ss.ptr);
			return -1;
			break;
		}
	}

	return fd;
}





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


static bool resove_host(const char * host, uint16_t port, void * sockaddr, socklen_t *socklen) {
	struct hostent *ent;
	//FIXME error check
	ent = gethostbyname(host);
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


int tcp_connect(const char * host, uint16_t port, const char ** msg) {
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd < 0) {
		return -1;
	}
	struct sockaddr_in in;
	socklen_t socklen;
	sstring_t ss = sstring_for_init;

	if(!resove_host(host, port, &in, &socklen)) {
		// reslove host failed
		sstring_append(&ss, "resolve host failed");
		*msg = ss.ptr;
		return -1;
	}

	if(connect(fd, (struct sockaddr *)&in, socklen) < 0) {
		sstring_fappend(&ss, "%d: %s", errno, strerror(errno));
		return -1;
	}
	return fd;
}





/*
 * resolve.c
 *
 *  Created on: Sep 19, 2012
 *      Author: hujin
 */

#include <stdio.h>
#include <netdb.h>
#include <unistd.h>


int main(int argc, char **argv) {

	if(argc < 2) {
		return 1;
	}

	struct hostent * ent;
	ent = gethostbyname(argv[1]);

	printf("h_name: %s\n", ent->h_name);
	int i = 0;
	char buf[1000] = {0};
	while(ent->h_addr_list[i]) {
		inet_ntop(AF_INET, ent->h_addr_list[i ++], buf, sizeof(buf));
		printf("addr list: %s\n", buf);
	}
	i = 0;
	while(ent->h_aliases[i]) {
		printf("aliases: %s\n", ent->h_aliases[i ++]);
	}

	return 0;
}

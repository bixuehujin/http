/*
 ============================================================================
 Name        : http2.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "http_conn.h"
#include "url.h"
#include "slist.h"

int main(void) {

	http_conn_t * conn = http_conn_new("localhost", 80);
	slist_t * slist = slist_new(int, NULL);

	url_t * url = url_parse("http://hujin@www.baidu.com:80/index.php?a=b&c=d#abc");
	printf("schema:  %s\n", url->scheme);
	printf("host:    %s\n", url->host);
	printf("port:    %d\n", url->port);
	printf("path:    %s\n", url->path);
	printf("user:    %s\n", url->user);
	printf("pass:    %s\n", url->pass);
	printf("query:   %s\n", url->query);
	printf("fragment:%s\n", url->fragment);
	url_free(url);
	return 0;
}

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

#include <clib.h>

#include "http_conn.h"
#include "http_request.h"
#include "url.h"


void on_state_change(int state, pointer data) {
	http_request_t * req = data;
	printf("State Changed: %d %s\n", state, state_names[state]);
	if(state == STATE_HEADERS_RECEIVED) {
		printf("Response: %d %s\n", req->status, req->status_txt);
	}
}

void on_load(char * res, pointer data) {
	printf("Response Loaded:\n");
	//printf("%s\n", res);
	http_request_t * req = data;

	printf("Response Headers:\n");
	printf("%s\n", req->res_header.ptr);

	hash_table_t * ht = http_request_parse_response_header(req);
	char * value =  hash_table_find(ht, "Content-Length");
	printf("dddddddddddddddddddddd   %s\n", value);
	hash_table_free(ht);
}


int main(int argc, char * argv[]) {

	if(argc < 2) {
		printf("Usage: http url\n");
		exit(1);
	}
	clib_init();


	http_request_t * req ;
	req = http_request_new(argv[1]);
	http_request_set_method(req, METHOD_GET);
	http_request_set_version(req, "1.1");

	http_request_on_state_change(req, on_state_change, req);
	http_request_on_load(req, on_load, req);
	http_request_preform(req);

	http_request_free(req);
	return 0;
}
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

	char * content_length = http_request_get_header(req, "Content-Length");

	printf("Content-Length: %s\n", content_length);

	printf("contents:\n%s\n", req->response.ptr);
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

	http_request_add_header(req, "Host", "baidu.com");
	http_request_add_header(req, "Connection", "close");
	http_request_add_header(req, "Accept", "*/*");

	http_request_on_state_change(req, on_state_change, req);
	http_request_on_load(req, on_load, req);
	if(!http_request_preform(req)) {
		http_request_print_error(req);
	}

	http_request_free(req);
	return 0;
}

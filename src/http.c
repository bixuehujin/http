/*
 ============================================================================
 Name        : http2.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "http.h"
#include "http_conn.h"
#include "http_message.h"
#include "http_request.h"
#include "url.h"


void on_state_change(http_message_t *res, int state, pointer data) {
	http_request_t * req = data;
	printf("State Changed: %d %s\n", state, state_names[state]);
	if(state == STATE_HEADERS_RECEIVED) {
		printf("Response: %d %s\n", res->status, res->status_txt);
	}
}

void on_load(http_message_t * res, pointer data) {
	printf("Response Loaded:\n");
	//printf("%s\n", res);
	//http_request_t * req = data;

	printf("Response Headers:\n");
	fprintf(stderr, "%s\n", res->raw_headers.ptr);

	//char * content_length = http_request_get_header(req, "Content-Length");

	printf("Content-Length: %d\n\n\n", res->length);

	//printf("contents:\n%s\n", res->body.ptr);


	hash_table_t * ht = res->headers;
	char * key, * value;
	hash_table_rewind(ht);
	while(hash_table_current(ht)) {
		printf("%s: %s\n", hash_table_current_key(ht), hash_table_current_data(ht));
		hash_table_next(ht);
	}

	http_message_free(res);
}


int main(int argc, char * argv[]) {

	if(argc < 2) {
		printf("Usage: %s url\n", argv[0]);
		exit(1);
	}
	clib_init();

	http_message_t * message = http_message_new(MESSAGE_REQUEST);
	http_message_set_url(message, argv[1]);
	http_message_add_header(message, "Host", "baidu.com");
	http_message_add_header(message, "Connection", "close");

	http_request_t * req ;
	req = http_request_new();
	http_request_add_message(req, &message);

	http_request_on_state_change(req, on_state_change, req);
	http_request_on_load(req, on_load, req);
	if(!http_request_run(req)) {
		http_request_print_error(req);
	}

	http_request_free(req);
	return 0;
}

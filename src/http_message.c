/*
 * http_message.c
 *
 *  Created on: Sep 21, 2012
 *      Author: hujin
 */

#include "http.h"
#include "http_message.h"


http_message_t * http_message_new(uint direction) {
	http_message_t * ret = malloc(sizeof(http_message_t));
	ret->url = NULL;
	ret->ver = strdup("1.1");
	ret->flags = 0;
	ret->direction = direction;
	ret->length = 0; /* content length of message */
	ret->headers = hash_table_new(20, free);
	ret->status = 0;
	ret->status_txt = NULL;
	ret->method = METHOD_GET;
	ret->req = NULL;

	sstring_init(&ret->raw_headers, 512);
	sstring_init(&ret->body, 1024);
	return ret;
}


void http_message_set_url(http_message_t *msg, const char * url) {
	if(msg->url) {
		url_free(msg->url);
	}
	msg->url = url_parse(url);
	url_dump(msg->url);
}


void http_message_set_method(http_message_t *msg, uint method) {
	msg->method = method;
}


void http_message_set_version(http_message_t *msg, const char * ver) {
	msg->ver = strdup(ver);
}


char * http_message_get_header(http_message_t * msg, const char * name) {
	return hash_table_find(msg->headers, name);
}


void http_message_add_header(http_message_t *msg, const char * name, const char * value) {
	sstring_fappend(&msg->raw_headers, "%s: %s\r\n", name, value);
}


void http_message_free(http_message_t * msg) {
	assert(msg != NULL);
	if(msg->url)
		url_free(msg->url);
	if(msg->ver)
		free(msg->ver);
	if(msg->headers)
		hash_table_free(msg->headers);
	if(msg->status_txt)
		free(msg->status_txt);

	sstring_destroy(&msg->raw_headers);
	sstring_destroy(&msg->body);

	if(msg->req) {
		http_message_free(msg->req);
	}

	free(msg);
}

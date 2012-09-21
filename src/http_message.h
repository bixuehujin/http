/*
 * http_message.h
 *
 *  Created on: Sep 21, 2012
 *      Author: hujin
 */

#ifndef HTTP_MESSAGE_H_
#define HTTP_MESSAGE_H_

#include "http.h"
#include "url.h"

/*

HOST:
Location:
From:
Refer:


Accept:
Accept-Charset:
Accept-Encoding:
Accept-Language:
Accept-Range:

Content-Encoding:
Content-Language:
Content-Length:
Content-Location:
Content-MD5
Content-Range:
Content-Type:
 */

/* flags for messages */
#define HTTP_CAN_GET_LENGTH 				1<<0
#define HTTP_CONN_REUSE						1<<1


/* message types */
#define MESSAGE_REQUEST						1
#define MESSAGE_RESPONSE					2


typedef struct _http_message {
	uint id; /* identifier of the message, must be unique */
	char * ver;
	url_t * url;
	uint flags;
	int direction;
	uint length;
	hash_table_t * headers;
	sstring_t  raw_headers;
	uint status; /* status code */
	char * status_txt;
	uint method; /* request type GET, POST, etc... */
	sstring_t body;
	struct _http_message * req; /* the request message */
}http_message_t;


http_message_t * http_message_new(uint direction);
void http_message_set_url(http_message_t *msg, const char * url);
void http_message_set_method(http_message_t *msg, uint type);
void http_message_set_version(http_message_t *msg, const char * ver);
char * http_message_get_header(http_message_t * msg, const char * name);
void http_message_add_header(http_message_t *msg, const char * name, const char * value);
void http_message_free(http_message_t * msg);

void http_message_process(http_message_t *msg);

#endif /* HTTP_MESSAGE_H_ */

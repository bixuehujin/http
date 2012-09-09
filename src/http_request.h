/*
 * http_request.h
 *
 *  Created on: Sep 8, 2012
 *      Author: hujin
 */

#ifndef HTTP_REQUEST_H_
#define HTTP_REQUEST_H_

#include "helper.h"
#include "sstring.h"
#include "http_conn.h"
#include "slist.h"

#define METHOD_HEAD 	0
#define METHOD_GET 		1
#define METHOD_POST 	2
#define METHOD_PUT 		3
#define METHOD_DELETE 	4

static char * method_names[] = {
	"HEAD",
	"GET",
	"POST",
	"PUT",
	"DELETE",
	NULL
};

typedef signed char http_method_t;


#define STATE_UNSENT			0
#define STATE_OPENED			1
#define STATE_HEADERS_RECEIVED	2
#define STATE_LOADING			3
#define STATE_DONE				4

static char * state_names[] = {
	"UNSENT",
	"OPENED",
	"HEADERS RECEIVED",
	"LOADING",
	"DONE",
	NULL
};

typedef signed char http_state_t;


typedef void (*http_request_cb_t)();
typedef void (*http_state_change_func_t)(int state, pointer user_data);
typedef void (*http_load_func_t)(char * res, pointer user_data);
typedef void (*http_error_func_t)(int state, pointer user_data);
typedef void (*http_progress_func_t)(int state, pointer user_data);
typedef void (*http_loadstart_func_t)(pointer user_data);
typedef void (*http_timeout_func_t)(pointer user_data);

typedef struct _http_request {
	/*request info*/
	char * url;
	char * uri;
	char * ver;
	unsigned char  method;
	http_conn_t * conn;
	sstring_t   header;

	/*handlers*/
	http_loadstart_func_t on_loadstart;
	http_load_func_t on_load;
	http_error_func_t on_error;
	http_progress_func_t on_progress;
	http_timeout_func_t on_timeout;
	http_state_change_func_t on_state_change;
	pointer load_data;
	pointer error_data;
	pointer progress_data;
	pointer state_change_data;
	pointer timeout_data;
	pointer loadstart_data;

	/*response message*/
	int status;
	char *  status_txt;
	sstring_t  res_header;
	sstring_t  response;

	/*ready state*/
	http_state_t state;
}http_request_t;

http_request_t * http_request_new(const char * url);
void http_request_free(http_request_t * req);

void http_request_set_url(http_request_t * req, const char * uri);
void http_request_set_version(http_request_t * req, const char * ver);
void http_request_set_method(http_request_t * req, http_method_t method);
void http_request_add_header(http_request_t * req, const char * name, const char * value);

void http_request_on_load(http_request_t * req, http_load_func_t cb, pointer user_data);
void http_request_on_error(http_request_t * req, http_error_func_t cb, pointer user_data);
void http_request_on_progress(http_request_t * req, http_progress_func_t cb, pointer user_data);
void http_request_on_state_change(http_request_t * req, http_state_change_func_t cb, pointer user_data);
void http_request_on_timeout(http_request_t *req, http_timeout_func_t cb, pointer user_data);
void http_request_on_loadstart(http_request_t *req, http_loadstart_func_t cb, pointer user_data);

char * http_request_get_response_header(http_request_t * req, const char * name);
slist_t * http_request_get_response_headers(http_request_t * req);
int http_request_get_response_status(http_request_t * req);
char * http_request_get_response_status_txt(http_request_t * req);
char * http_request_get_response(http_request_t * req);

bool http_request_preform(http_request_t * req);

#endif /* HTTP_REQUEST_H_ */

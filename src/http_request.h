/*
 * http_request.h
 *
 *  Created on: Sep 8, 2012
 *      Author: hujin
 */

#ifndef HTTP_REQUEST_H_
#define HTTP_REQUEST_H_

#include "errno.h"
#include "clib.h"
#include "http_conn.h"
#include "http_message.h"

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
typedef void (*http_state_change_func_t)	(http_message_t * res, int state, pointer user_data);
typedef void (*http_load_func_t)			(http_message_t * res, pointer user_data);
typedef void (*http_error_func_t)			(http_message_t * res, pointer user_data);
typedef void (*http_progress_func_t)		(http_message_t * res, size_t complete, size_t total, pointer user_data);
typedef void (*http_loadstart_func_t)		(http_message_t * res, pointer user_data);
typedef void (*http_timeout_func_t)			(pointer user_data);

typedef struct _http_request_handlers {
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
}http_request_handlers_t;


typedef struct _http_request {
	http_conn_t * conn;
	http_request_handlers_t handlers;

	/*ready state*/
	http_state_t state;

	/*hash table of headers*/
	hash_table_t * ht_headers;

	slist_t * messages; /* message wait for process */

	/* errors */
	cerror_t * error;
}http_request_t;

#define http_request_on_ex(request, e, func, user_data) \
	http_request_on_ ## e(request, func, user_data)

#define http_request_on(request, e, func) \
	http_request_on_ ## e(request, func, NULL)


http_request_t * http_request_new();
void http_request_free(http_request_t * req);

/* events */
void http_request_on_load(http_request_t * req, http_load_func_t cb, pointer user_data);
void http_request_on_error(http_request_t * req, http_error_func_t cb, pointer user_data);
void http_request_on_progress(http_request_t * req, http_progress_func_t cb, pointer user_data);
void http_request_on_state_change(http_request_t * req, http_state_change_func_t cb, pointer user_data);
void http_request_on_timeout(http_request_t *req, http_timeout_func_t cb, pointer user_data);
void http_request_on_loadstart(http_request_t *req, http_loadstart_func_t cb, pointer user_data);

/* perform */
bool http_request_preform(http_request_t * req);

/* error handle */
int http_request_get_error(http_request_t * req, char * error);
void http_request_print_error(http_request_t * req);

void http_request_add_message(http_request_t * req, http_message_t **msg);

#endif /* HTTP_REQUEST_H_ */

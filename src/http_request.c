/*
 * http_request.c
 *
 *  Created on: Sep 8, 2012
 *      Author: hujin
 */

#include "http.h"
#include "url.h"
#include "http_conn.h"
#include "http_message.h"
#include "http_request.h"



static bool http_request_init(http_request_t *req) {

	http_conn_pool_start();

	req->ht_headers = hash_table_new(20, free);
	req->messages = slist_new(http_message_t *, NULL);
	req->error = NULL;
	return true;
}

/**
 *
 */
static bool readline(int fd, sstring_t *ret) {
	char buffer[2] = {0};
	size_t n;
	while((n = read(fd, buffer, 1)) >= 0) {
		if(n == 0) {
			return false;
		}
		if(buffer[0] == '\r') {
			read(fd, buffer, 1);
			break;
		}
		sstring_appendc(ret, buffer[0]);
	}
	return true;
}


static char  * parse_header(sstring_t *ss, char * label) {
	int i = 0;
	char * str = ss->ptr;
	for(; *str != ':'; label ++, str ++) {
		*label = *str;
	}
	*label = '\0';
	str++;
	for(;*str == ' '; str ++);
	return strdup(str);
}


static void change_state(http_request_t * req,http_message_t *msg, http_state_t state) {
	if(req->handlers.on_state_change) {
		req->handlers.on_state_change(msg, state, req->handlers.state_change_data);
	}
}


http_request_t * http_request_new() {
	http_request_t * ret = m_new0(http_request_t, 1);
	assert(ret != NULL);

	ret->handlers.on_load = NULL;
	ret->handlers.on_error = NULL;
	ret->handlers.on_progress = NULL;
	ret->handlers.on_state_change = NULL;
	ret->handlers.on_timeout = NULL;
	ret->handlers.on_loadstart = NULL;
	ret->state = STATE_UNSENT;


	http_request_init(ret);

	return ret;
}


inline void http_request_on_load(http_request_t * req, http_load_func_t cb, pointer data) {
	assert(req != NULL);
	req->handlers.on_load = cb;
	req->handlers.load_data = data;
}


inline void http_request_on_error(http_request_t * req, http_error_func_t cb, pointer data) {
	assert(req != NULL);
	req->handlers.on_error = cb;
	req->handlers.error_data = data;
}


inline void http_request_on_progress(http_request_t * req, http_progress_func_t cb, pointer data) {
	assert(req != NULL);
	req->handlers.on_progress = cb;
	req->handlers.progress_data = data;
}


inline inline void http_request_on_state_change(http_request_t * req, http_state_change_func_t cb, pointer user_data) {
	assert(req != NULL);
	req->handlers.on_state_change = cb;
	req->handlers.state_change_data = user_data;
}


inline void http_request_on_timeout(http_request_t *req, http_timeout_func_t cb, pointer user_data) {
	assert(req != NULL);
	req->handlers.on_timeout = cb;
	req->handlers.timeout_data = user_data;
}


inline void http_request_on_loadstart(http_request_t *req, http_loadstart_func_t cb, pointer user_data) {
	assert(req != NULL);
	req->handlers.on_loadstart = cb;
	req->handlers.loadstart_data = user_data;
}


static void parse_status(http_message_t * msg, sstring_t * line) {
	char * fstr = NULL;

	fstr = memchr(line->ptr, ' ', line->len);
	msg->status = atoi(++fstr);

	fstr = memchr(fstr, ' ', strlen(fstr));
	msg->status_txt = strdup(fstr);
}


static void fetch_header(http_request_t * req, http_conn_t * conn, http_message_t * msg) {
	int response_line = 1;
	sstring_t  line = sstring_for_init;
	while((readline(conn->connfd, &line)) && !sstring_empty(&line)) {
		if(response_line == 1) {
			parse_status(msg, &line);
			response_line ++;
			sstring_clear(&line);
			continue;
		}
		if(strcmp(line.ptr, "\r") == 0) {
			break;
		}
		sstring_fappend(&msg->raw_headers, "%s\n", line.ptr);
		char name[200] = {0}, * value;
		value = parse_header(&line, name);
		if(value) {
			hash_table_insert(msg->headers, name, value);
		}
		if(strcmp(name, "Content-Length") == 0) {
			msg->length = atoi(value);
		}
		response_line ++;
		sstring_clear(&line);
	}

	//headers received
	change_state(req, msg, STATE_HEADERS_RECEIVED);
	sstring_destroy(&line);
}


/**
 * build a request header form http_message object .
 */
static void build_request_header(http_message_t *msg, sstring_t *ss) {
	char * uri = url_get_uri(msg->url);
	sstring_fappend(ss,
					"%s %s HTTP/%s\r\n",
					method_names[msg->method],
					uri ? uri : "/",
					msg->ver
		);
	free(uri);
	if(!sstring_empty(&msg->raw_headers)) {
		sstring_append(ss, msg->raw_headers.ptr);
	}

	sstring_appendc(ss, '\n');
}


bool http_request_run(http_request_t * req) {
	sstring_t req_header = sstring_for_init;
	http_message_t * req_message = (pointer)(*(size_t *)slist_head(req->messages));
	http_message_t * res_message = NULL;
	cerror_t * error = NULL;
	char * buffer[200] = {0};

	build_request_header(req_message, &req_header);

	http_conn_t * conn = http_conn_pool_get_conn(req_message->url->host, req_message->url->port ? req_message->url->port : 80, &error);
	if(!conn) {
		req->error = error;
		sstring_destroy(&req_header);
		return false;
	}

	res_message = http_message_new(MESSAGE_RESPONSE);
	res_message->req = req_message;

	change_state(req, res_message, STATE_OPENED);


	int n = write(conn->connfd, req_header.ptr, req_header.len);

	fetch_header(req, conn, res_message);

	if(req_message->method != METHOD_HEAD) {
		change_state(req, res_message, STATE_LOADING);
		size_t complete = 0;
		size_t total = 0;
		char * content_length = http_message_get_header(res_message, "Content-Length");
		if(content_length) {
			total = atoi(content_length);
		}
		if(req->handlers.on_loadstart) {
			req->handlers.on_loadstart(res_message, req->handlers.loadstart_data);
		}
		if(req->handlers.on_progress) {
			req->handlers.on_progress(res_message, complete, total ,req->handlers.progress_data);
		}

		while((n = read(conn->connfd, buffer, 100))) {
			if(n < 0 ) {
				//error handle there
				break;
			}else {
				sstring_appendl(&res_message->body, buffer, n);
				complete += n;
				if(req->handlers.on_progress) {
					req->handlers.on_progress(res_message, complete, total, req->handlers.progress_data);
				}
			}
		}
		if(req->handlers.on_load) {
			req->handlers.on_load(res_message, req->handlers.load_data);
		}
	}

	change_state(req, res_message, STATE_DONE);

	sstring_destroy(&req_header);
	return true;
}


void http_request_free(http_request_t * req) {
	assert(req != NULL);

	hash_table_free(req->ht_headers);
	if(req->error) {
		cerror_free(&req->error);
	}
	slist_free(req->messages);
	free(req);
	http_conn_pool_shutdown();
}


void http_request_print_error(http_request_t * req) {
	cerror_print(req->error);
}


void http_request_add_message(http_request_t * req, http_message_t **msg) {
	slist_append(req->messages, msg);
}

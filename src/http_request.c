/*
 * http_request.c
 *
 *  Created on: Sep 8, 2012
 *      Author: hujin
 */

#include <string.h>
#include <assert.h>
#include "url.h"
#include "http_request.h"



static bool http_request_init(http_request_t *req) {
	url_t * url = url_parse(req->url);
	char * uri = url_get_uri(url);
	req->uri = uri ? strdup(uri) : strdup("/");
	if(uri) {
		free(uri);
	}

	http_conn_t * conn = http_conn_new(url->host, url->port ? url->port : 80);
	assert(conn != NULL);
	if(!conn) {
		return false;
	}
	req->conn = conn;
	url_free(url);

	sstring_init(&req->header, 200);
	sstring_init(&req->res_header, 512);
	sstring_init(&req->response, 2048);

	req->ht_headers = hash_table_new(20, free);

	return true;
}

/**
 *
 */
static sstring_t * readline(int fd) {
	sstring_t * ret = sstring_new(100);
	char buffer[2] = {0};
	size_t n;
	while((n = read(fd, buffer, 1)) >= 0) {
		if(n == 0) {
			return NULL;
		}
		if(buffer[0] == '\n') {
			break;
		}
		sstring_append(ret, buffer);
	}
	return ret;
}



static void parse_status(http_request_t * req, sstring_t * line) {
	char * fstr = NULL;

	fstr = memchr(line->ptr, ' ', line->len);
	req->status = atoi(++fstr);

	fstr = memchr(fstr, ' ', strlen(fstr));
	req->status_txt = strdup(fstr);
	printf("status: %s\n", line->ptr);
}


static char  * parse_header(sstring_t *ss, char * label) {
	char * name = strtok(ss->ptr, ":");
	char * value = strtok(NULL, ":");
	if(!name || !value ) {
		return NULL;
	}
	strcpy(label, name);
	return strdup(value);
}

static void change_state(http_request_t * req, http_state_t state) {
	if(req->handlers.on_state_change) {
		req->handlers.on_state_change(state, req->handlers.state_change_data);
	}
}


http_request_t * http_request_new(const char * url) {
	http_request_t * ret = m_new0(http_request_t, 1);
	assert(ret != NULL);
	ret->url = strdup(url);

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


inline void http_request_set_uri(http_request_t * req, const char * uri) {
	req->uri = strdup(uri);
}


inline void http_request_set_version(http_request_t * req, const char * ver) {
	req->ver = strdup(ver);
}


inline void http_request_set_method(http_request_t * req, http_method_t method) {
	req->method = method;
}


inline void http_request_add_header(http_request_t * req, const char * name, const char * value) {
	sstring_fappend(&req->header, "%s: %s\r\n", name, value);
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


inline char * http_request_get_header(http_request_t * req, const char * name) {
	return hash_table_find(req->ht_headers, name);
}


bool http_request_preform(http_request_t * req) {
	sstring_t ss, *pss = NULL, nss;
	sstring_init(&ss, 100);
	sstring_init(&nss, 100);
	char buffer[200] = {0};
	int response_line = 1;
	pss = &ss;

	int response_body_started = 0;

	sstring_fappend(pss,
				"%s %s HTTP/%s\r\n",
				method_names[req->method],
				req->uri,
				req->ver
	);
	if(!sstring_empty(&req->header)) {
		sstring_append(pss, req->header.ptr);
	}

	sstring_appendc(pss, '\n');

	if (!http_conn_connect(req->conn)) {
		return false;
	}
	// trigger STATE_OPENED.
	change_state(req, STATE_OPENED);

	//printf("request header: %s len:%d\n", pss->ptr, pss->len);
	int n = write(req->conn->connfd, pss->ptr, pss->len);


	sstring_t * line = NULL;
	while((line = readline(req->conn->connfd)) && !sstring_empty(line)) {
		if(response_line == 1) {
			parse_status(req, line);
		}
		if(strcmp(line->ptr, "\r") == 0) {
			response_body_started = 1;
			break;
		}
		sstring_append(&req->res_header, line->ptr);
		//printf("header:%s\n", line->ptr);

		char name[200] = {0}, * value;
		value = parse_header(line, name);
		if(value) {
			hash_table_insert(req->ht_headers, name, value);
		}

		sstring_free(line);
		response_line ++;
	}

	//headers received
	change_state(req, STATE_HEADERS_RECEIVED);

	if(line) {
		sstring_free(line);
	}

	if(response_body_started && req->method != METHOD_HEAD) {
		change_state(req, STATE_LOADING);
		size_t complete = 0;
		size_t total = 0;
		char * content_length = http_request_get_header(req, "Content-Length");
		if(content_length) {
			total = atoi(content_length);
		}
		if(req->handlers.on_loadstart) {
			req->handlers.on_loadstart(req->handlers.loadstart_data);
		}
		if(req->handlers.on_progress) {
			req->handlers.on_progress(complete, total ,req->handlers.progress_data);
		}

		while((n = read(req->conn->connfd, buffer, 100))) {
			if(n < 0 ) {
				//error handle there
				break;
			}else {
				sstring_appendl(&req->response, buffer, n);
				complete += n;
				if(req->handlers.on_progress) {
					req->handlers.on_progress(complete, total, req->handlers.progress_data);
				}
			}
		}
		if(req->handlers.on_load) {
			req->handlers.on_load(buffer, req->handlers.load_data);
		}
	}

	change_state(req, STATE_DONE);

	sstring_destroy(pss);
	sstring_destroy(&nss);

	return true;
}


void http_request_free(http_request_t * req) {
	assert(req != NULL);
	http_conn_free(req->conn);

	sstring_destroy(&req->header);
	sstring_destroy(&req->res_header);
	sstring_destroy(&req->response);
	free(req->status_txt);
	if(req->uri) {
		free(req->uri);
	}
	hash_table_free(req->ht_headers);
	free(req->url);
	free(req->ver);
	free(req);
}


char * http_request_get_response_header(http_request_t * req, const char * name) {
	return req->res_header.ptr;
}


hash_table_t * http_request_parse_response_header(http_request_t * req) {

	if(sstring_empty(&req->res_header)) {
		return NULL;
	}

	hash_table_t * ret = hash_table_new(30, free);
	char * header = req->res_header.ptr;
	char * name, * value, *line;
	char * saveptr1, * saveptr2;

	line = strtok_r(header, "\r", &saveptr1);
	while(line) {
		line = strtok_r(NULL, "\r", &saveptr1);
		if(!line) break;
		name = strtok_r(line, ":", &saveptr2);
		value = strtok_r(NULL, ":", &saveptr2);
		hash_table_insert(ret, name, strdup(value));
	}

	return ret;
}


inline int http_request_get_response_status(http_request_t * req) {
	return req->status;
}


inline char * http_request_get_response_status_txt(http_request_t * req) {
	return req->status_txt;
}


inline char * http_request_get_response(http_request_t * req) {
	return req->response.ptr;
}



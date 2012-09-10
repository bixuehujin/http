/*
 * curl.c
 *
 *  Created on: Sep 9, 2012
 *      Author: hujin
 */

#include <unistd.h>
#include <getopt.h>
#include <assert.h>
#include "http_request.h"
#include "curl.h"
#include "clib.h"


curl_t * curl_new() {
	curl_t * ret = m_new0(curl_t, 1);
	assert(ret != NULL);
	ret->urls = slist_new(string, NULL);
	return ret;
}


void curl_add_request_url(curl_t * curl, const char * url) {
	assert(curl != NULL);
	printf("url:%s\n", url);
	slist_append(curl->urls, &url);
}
void curl_run(curl_t * curl) {
		slist_node_t * node = curl->urls->head;
	while(node) {
		printf("%s\n", *(string *)*node->data);
		node = node->next;
	}
}


void curl_free(curl_t * curl) {
	assert(curl != NULL);
	slist_free(curl->urls);
	free(curl);
}

/*
 * curl.h
 *
 *  Created on: Sep 9, 2012
 *      Author: hujin
 */

#ifndef CURL_H_
#define CURL_H_
#include "clib.h"

typedef struct _curl {
	slist_t * urls;
}curl_t;


curl_t * curl_new();
void curl_add_request_url(curl_t * curl, const char * url);
void curl_run(curl_t * curl);
void curl_free(curl_t * curl);

#endif /* CURL_H_ */

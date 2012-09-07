/*
 * url.h
 *
 *  Created on: 2012-9-6
 *      Author: hujin
 */

#ifndef URL_H_
#define URL_H_



typedef struct _url {
	char *scheme;
	char *user;
	char *pass;
	char *host;
	unsigned short port;
	char *path;
	char *query;
	char *fragment;
}url_t;

#define url_parse(url) _url_parse(url, strlen(url))
void url_free(url_t * url);


#endif /* URL_H_ */

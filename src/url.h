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

url_t * url_parse(const char * url);

char * url_get_uri(url_t * url);
url_t *_url_parse(char const *str, int length);
void url_dump(url_t * url);
void url_free(url_t * url);


#endif /* URL_H_ */

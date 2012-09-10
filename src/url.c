/*
 * url.c
 *
 *  Created on: 2012-9-6
 *      Author: hujin
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include <clib.h>

#include "url.h"

static inline const void *zend_memrchr(const void *s, int c, size_t n){
	register const unsigned char *e;

	if (n <= 0) {
		return NULL;
	}

	for (e = (const unsigned char *)s + n - 1; e >= (const unsigned char *)s; e--) {
		if (*e == (const unsigned char)c) {
			return (const void *)e;
		}
	}

	return NULL;
}


char *replace_controlchars(char *str, int len){
	unsigned char *s = (unsigned char *)str;
	unsigned char *e = (unsigned char *)str + len;

	if (!str) {
		return (NULL);
	}

	while (s < e) {

		if (iscntrl(*s)) {
			*s='_';
		}
		s++;
	}

	return (str);
}


/**
 * this the parse_url() implements in php-src.
 * more information see:http://www.php.net
 */
url_t *_url_parse(char const *str, int length){
	char port_buf[6];
	url_t *ret = m_new(url_t, 1);
	char const *s, *e, *p, *pp, *ue;

	s = str;
	ue = s + length;

	/* parse scheme */
	if ((e = memchr(s, ':', length)) && (e - s)) {
		/* validate scheme */
		p = s;
		while (p < e) {
			/* scheme = 1*[ lowalpha | digit | "+" | "-" | "." ] */
			if (!isalpha(*p) && !isdigit(*p) && *p != '+' && *p != '.' && *p != '-') {
				if (e + 1 < ue) {
					goto parse_port;
				} else {
					goto just_path;
				}
			}
			p++;
		}

		if (*(e + 1) == '\0') { /* only scheme is available */
			ret->scheme = strndup(s, (e - s));
			replace_controlchars(ret->scheme, (e - s));
			goto end;
		}

		/*
		 * certain schemas like mailto: and zlib: may not have any / after them
		 * this check ensures we support those.
		 */
		if (*(e+1) != '/') {
			/* check if the data we get is a port this allows us to
			 * correctly parse things like a.com:80
			 */
			p = e + 1;
			while (isdigit(*p)) {
				p++;
			}

			if ((*p == '\0' || *p == '/') && (p - e) < 7) {
				goto parse_port;
			}

			ret->scheme = strndup(s, (e-s));
			replace_controlchars(ret->scheme, (e - s));

			length -= ++e - s;
			s = e;
			goto just_path;
		} else {
			ret->scheme = strndup(s, (e-s));
			replace_controlchars(ret->scheme, (e - s));

			if (*(e+2) == '/') {
				s = e + 3;
				if (!strncasecmp("file", ret->scheme, sizeof("file"))) {
					if (*(e + 3) == '/') {
						/* support windows drive letters as in:
						   file:///c:/somedir/file.txt
						*/
						if (*(e + 5) == ':') {
							s = e + 4;
						}
						goto nohost;
					}
				}
			} else {
				if (!strncasecmp("file", ret->scheme, sizeof("file"))) {
					s = e + 1;
					goto nohost;
				} else {
					length -= ++e - s;
					s = e;
					goto just_path;
				}
			}
		}
	} else if (e) { /* no scheme; starts with colon: look for port */
		parse_port:
		p = e + 1;
		pp = p;

		while (pp-p < 6 && isdigit(*pp)) {
			pp++;
		}

		if (pp - p > 0 && pp - p < 6 && (*pp == '/' || *pp == '\0')) {
			long port;
			memcpy(port_buf, p, (pp - p));
			port_buf[pp - p] = '\0';
			port = strtol(port_buf, NULL, 10);
			if (port > 0 && port <= 65535) {
				ret->port = (unsigned short) port;
			} else {
				free(ret->scheme);
				free(ret);
				return NULL;
			}
		} else if (p == pp && *pp == '\0') {
			free(ret->scheme);
			free(ret);
			return NULL;
		} else {
			goto just_path;
		}
	} else {
		just_path:
		ue = s + length;
		goto nohost;
	}

	e = ue;

	if (!(p = memchr(s, '/', (ue - s)))) {
		char *query, *fragment;

		query = memchr(s, '?', (ue - s));
		fragment = memchr(s, '#', (ue - s));

		if (query && fragment) {
			if (query > fragment) {
				p = e = fragment;
			} else {
				p = e = query;
			}
		} else if (query) {
			p = e = query;
		} else if (fragment) {
			p = e = fragment;
		}
	} else {
		e = p;
	}

	/* check for login and password */
	if ((p = zend_memrchr(s, '@', (e-s)))) {
		if ((pp = memchr(s, ':', (p-s)))) {
			if ((pp-s) > 0) {
				ret->user = strndup(s, (pp-s));
				replace_controlchars(ret->user, (pp - s));
			}

			pp++;
			if (p-pp > 0) {
				ret->pass = strndup(pp, (p-pp));
				replace_controlchars(ret->pass, (p-pp));
			}
		} else {
			ret->user = strndup(s, (p-s));
			replace_controlchars(ret->user, (p-s));
		}

		s = p + 1;
	}

	/* check for port */
	if (*s == '[' && *(e-1) == ']') {
		/* Short circuit portscan,
		   we're dealing with an
		   IPv6 embedded address */
		p = s;
	} else {
		/* memrchr is a GNU specific extension
		   Emulate for wide compatability */
		for(p = e; *p != ':' && p >= s; p--);
	}

	if (p >= s && *p == ':') {
		if (!ret->port) {
			p++;
			if (e-p > 5) { /* port cannot be longer then 5 characters */
				free(ret->scheme);
				free(ret->user);
				free(ret->pass);
				free(ret);
				return NULL;
			} else if (e - p > 0) {
				long port;
				memcpy(port_buf, p, (e - p));
				port_buf[e - p] = '\0';
				port = strtol(port_buf, NULL, 10);
				if (port > 0 && port <= 65535) {
					ret->port = (unsigned short)port;
				} else {
					free(ret->scheme);
					free(ret->user);
					free(ret->pass);
					free(ret);
					return NULL;
				}
			}
			p--;
		}
	} else {
		p = e;
	}

	/* check if we have a valid host, if we don't reject the string as url */
	if ((p-s) < 1) {
		free(ret->scheme);
		free(ret->user);
		free(ret->pass);
		free(ret);
		return NULL;
	}

	ret->host = strndup(s, (p-s));
	replace_controlchars(ret->host, (p - s));

	if (e == ue) {
		return ret;
	}

	s = e;

	nohost:

	if ((p = memchr(s, '?', (ue - s)))) {
		pp = strchr(s, '#');

		if (pp && pp < p) {
			if (pp - s) {
				ret->path = strndup(s, (pp-s));
				replace_controlchars(ret->path, (pp - s));
			}
			p = pp;
			goto label_parse;
		}

		if (p - s) {
			ret->path = strndup(s, (p-s));
			replace_controlchars(ret->path, (p - s));
		}

		if (pp) {
			if (pp - ++p) {
				ret->query = strndup(p, (pp-p));
				replace_controlchars(ret->query, (pp - p));
			}
			p = pp;
			goto label_parse;
		} else if (++p - ue) {
			ret->query = strndup(p, (ue-p));
			replace_controlchars(ret->query, (ue - p));
		}
	} else if ((p = memchr(s, '#', (ue - s)))) {
		if (p - s) {
			ret->path = strndup(s, (p-s));
			replace_controlchars(ret->path, (p - s));
		}

		label_parse:
		p++;

		if (ue - p) {
			ret->fragment = strndup(p, (ue-p));
			replace_controlchars(ret->fragment, (ue - p));
		}
	} else {
		ret->path = strndup(s, (ue-s));
		replace_controlchars(ret->path, (ue - s));
	}
end:
	return ret;
}


char * url_get_uri(url_t * url) {
	sstring_t ss ;
	sstring_init(&ss, 100);
	if(url->path) {
		sstring_append(&ss, url->path);
	}
	if(url->query) {
		sstring_sprintf_append(&ss, "?%s", url->query);
	}
	if(url->fragment) {
		sstring_sprintf_append(&ss, "#%s", url->fragment);
	}
	return ss.ptr;
}


void url_dump(url_t * url) {
	if(url->scheme) {
		printf("scheme:    %s\n", url->scheme);
	}
	if(url->host) {
		printf("host:      %s\n", url->host);
	}
	if(url->port) {
		printf("port:      %d\n", url->port);
	}
	if(url->user) {
		printf("user:      %s\n", url->user);
	}
	if(url->pass) {
		printf("pass:      %s\n", url->pass);
	}
	if(url->path) {
		printf("path:      %s\n", url->path);
	}
	if(url->query) {
		printf("query:     %s\n", url->query);
	}
	if(url->fragment) {
		printf("fragment:  %s\n", url->fragment);
	}
}


void url_free(url_t * url) {
	assert(url != NULL);
	if (url->scheme)
		free(url->scheme);
	if (url->user)
		free(url->user);
	if (url->pass)
		free(url->pass);
	if (url->host)
		free(url->host);
	if (url->path)
		free(url->path);
	if (url->query)
		free(url->query);
	if (url->fragment)
		free(url->fragment);

	free(url);
}

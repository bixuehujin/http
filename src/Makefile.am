bin_PROGRAMS=http
http_SOURCES=http.c \
	http_conn.c \
	url.c \
	http_request.c \
	tcp.c \
	http_message.c
	

http_CFLAGS=-L/usr/local/lib -lclib -I../lib/include 

http_LDADD=/usr/local/lib/libclib.a
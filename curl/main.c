/*
 * main.c
 *
 *  Created on: Sep 9, 2012
 *      Author: hujin
 */

#include <unistd.h>
#include <getopt.h>
#include "http_request.h"
#include "curl.h"

char ** request_urls = NULL;

static struct option options[] = {
		{"head", no_argument, 0, 'I'},
		{0, 0, 0, 0}
};


static void parse_args(int argc, char * argv[]) {

	while(1) {
		int option_index = 0;
		int o = getopt_long(argc, argv, "I", options, &option_index);

		if(o == -1) {
			break;
		}

		switch(o) {
		case 'I':

			break;

		case '?':

			break;

		default:

			break;
		}
	}
	if(optind < argc) {
		request_urls = &argv[optind];
	}else {
		printf("wrong argument\n");
		exit(1);
	}

}

int main(int argc, char * argv[]) {


	parse_args(argc, argv);
	curl_t * curl = curl_new();
	int i = 0;
	while(request_urls[i]) {
		curl_add_request_url(curl, request_urls[i ++]);
	}
	curl_run(curl);
	curl_free(curl);
	return 0;
}

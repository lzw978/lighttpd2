
#include "request.h"

request* request_new() {
	request *req = g_slice_new0(request);
	return req;
}

void request_free(request *req) {
	/* TODO */
	g_slice_free(request, req);
}
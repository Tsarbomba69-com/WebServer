#include <time.h>
#include "cweb.h"

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT 8080

void log() {
	printf("Serving on: http://localhost:%i\n", DEFAULT_PORT);
}

void business_rule(Request* req, Response* res) {
	printf("status code: %i\n", res->status_code);
	printf("METHOD: %s\n", req->method);
	printf("uri: %s\n", req->uri);
	printf("Host: %s\n", get_item(&(req->headers), (char*)"Host"));
	
	if (strcmp(req->method, "POST") == 0) {
		printf("morada: %s\n", (char*)get_item(&(req->body), (char*)"morada"));
	}
}



int main() {
	get("/", business_rule);
	post("/myPost", business_rule);
	start_server(DEFAULT_PORT, log);
	return EXIT_SUCCESS;
}
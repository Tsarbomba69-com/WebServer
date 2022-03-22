#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

typedef struct Request {
	char* method;
	char* uri;
	char* header;
	char* content_type;
	char* http_version;
	void* body;
	char* ip;
} Request;

typedef struct Response {
	int status_code;
	char* header;
	char* content_type;
	void* body;
	int content_length;
} Response;

int start_server(int PORT = 8080, void (*callback)(void) = NULL);

int get(const char* uri, SOCKET new_socket, void (*callback)(Request, Response));

char* render_template(const char* file_path);

struct Request handle_http_request(SOCKET new_socket);

void extract_request_line_fields(struct Request* request, char* request_line, size_t length);

struct Response send_response(SOCKET new_socket, const char* header, const char* content_type, void* body, int content_length);


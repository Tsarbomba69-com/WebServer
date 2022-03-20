#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

struct Request {
	char* method;
	char* path;
	char* header;
	char* content_type;
	void* body;
	char* ip;
};

struct Response {
	int status_code;
	char* header;
	char* content_type;
	void* body;
	int content_length;
};

int start_server(int PORT = 8080, void (*callback)(void) = NULL);

int get(const char* uri, SOCKET new_socket, void (*callback)(struct Request, struct Response));

char* render_template(const char* file_path);

struct Request handle_http_request(SOCKET new_socket);

struct Response send_response(SOCKET new_socket, const char* header, const char* content_type, void* body, int content_length);


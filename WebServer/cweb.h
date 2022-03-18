#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

int start_server(int PORT = 8080, void (*callback)(void) = NULL);

int get(const char* uri, SOCKET new_socket);

char* render_html(const char* file_path);

struct Request handle_http_request(SOCKET new_socket);



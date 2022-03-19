#include <time.h>
#include "cweb.h"

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "8026"

void log() {
	printf("Serving on: http://localhost:%i\n", 8801);
}



int main() {
	start_server(8080, log);
	SOCKET new_socket;
	int c;
	const char* message;

	//Accept and incoming connection
	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);
	extern SOCKET SERVER_SOCKET;
	extern SOCKET CLIENT_SOCKET;

	while ((new_socket = accept(SERVER_SOCKET, (struct sockaddr*)&CLIENT_SOCKET, &c)) != INVALID_SOCKET)
	{
		puts("Connection accepted");
		get("", new_socket);
		closesocket(new_socket);
		//request();
		//struct tm newtime;
		//time_t now = time(0);
		//localtime_s(&newtime, &now);
		//char date[255];

		//Send some data
		//message = "HTTP/1.1 200 OK\r\n"
		//	"Connection: keep-alive\r\n"
		//	"Content-Type: text/html\r\n"
		//	"Content-Length: 1024\r\n"
		//	"\r\n"
		//	"<html><head><title>MEU SERVIDOR</title></head><body><h1>MEU SERVIDOR CARALHO!</h1></body></html>";

		//if (send(new_socket, message, (int)strlen(message), 0) < 0)
		//{
		//	puts("Send failed");
		//	return EXIT_FAILURE;
		//}

		//printf("Serving on: http://localhost:%s\n", DEFAULT_PORT);
	}

	if (new_socket == INVALID_SOCKET)
	{
		printf("accept failed with error code : %d", WSAGetLastError());
		return EXIT_FAILURE;
	}

	//closesocket(SERVER_SOCKET);
	WSACleanup();

	return EXIT_SUCCESS;
}
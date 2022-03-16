#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "8026"

int main() {
	WSADATA wsaData;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return EXIT_FAILURE;
	}

	struct addrinfo* result = NULL, * ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return EXIT_FAILURE;
	}

	// Create a SOCKET for the server to listen for client connections
	SOCKET ServerSocket = INVALID_SOCKET;
	ServerSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ServerSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return EXIT_FAILURE;
	}

	iResult = bind(ServerSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ServerSocket);
		WSACleanup();
		return EXIT_FAILURE;
	}

	freeaddrinfo(result);

	if (listen(ServerSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Listen failed with error: %ld\n", WSAGetLastError());
		closesocket(ServerSocket);
		WSACleanup();
		return EXIT_FAILURE;
	}

	SOCKET ClientSocket, new_socket;
	ClientSocket = INVALID_SOCKET;
	int c;
	const char* message;


	//Accept and incoming connection
	puts("Waiting for incoming connections...");

	c = sizeof(struct sockaddr_in);

	while ((new_socket = accept(ServerSocket, (struct sockaddr*)&ClientSocket, &c)) != INVALID_SOCKET)
	{
		puts("Connection accepted");
		
		//struct tm newtime;
		//time_t now = time(0);
		//localtime_s(&newtime, &now);
		//char date[255];
		
		//Send some data
		message = "HTTP/1.1 201 KAY\r\n"
			"Connection: keep-alive\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: 1024\r\n"
			"\r\n"
			"<html><head><title>MEU SERVIDOR</title></head><body><h1>MEU SERVIDOR CARALHO!</h1></body></html>";

		if (send(new_socket, message, strlen(message), 0) < 0)
		{
			puts("Send failed");
			return EXIT_FAILURE;
		}
		printf("Serving on: http://localhost:%s\n", DEFAULT_PORT);
	}

	if (new_socket == INVALID_SOCKET)
	{
		printf("accept failed with error code : %d", WSAGetLastError());
		return EXIT_FAILURE;
	}

	closesocket(ServerSocket);
	WSACleanup();

	return EXIT_SUCCESS;
}
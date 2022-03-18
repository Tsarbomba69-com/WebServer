#include "cweb.h"

SOCKET SERVER_SOCKET, CLIENT_SOCKET = INVALID_SOCKET;

struct Request {
	char* header;
	char* content_type;
	void* body;
	int content_length;
};


int start_server(int PORT, void (*callback)(void)) {
	WSADATA wsaData;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return EXIT_FAILURE;
	}

	char* _PORT = (char*)malloc(32 * sizeof(char));
	sprintf_s(_PORT, sizeof(_PORT), "%d", PORT);
	struct addrinfo* result = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	iResult = getaddrinfo(NULL, _PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return EXIT_FAILURE;
	}

	// Create a SOCKET for the server to listen for client connections
	SERVER_SOCKET = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (SERVER_SOCKET == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return EXIT_FAILURE;
	}

	iResult = bind(SERVER_SOCKET, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(SERVER_SOCKET);
		WSACleanup();
		return EXIT_FAILURE;
	}

	freeaddrinfo(result);

	if (listen(SERVER_SOCKET, SOMAXCONN) == SOCKET_ERROR) {
		printf("Listen failed with error: %ld\n", WSAGetLastError());
		closesocket(SERVER_SOCKET);
		WSACleanup();
		return EXIT_FAILURE;
	}
	callback();
	return EXIT_SUCCESS;
}


int get(const char* uri, SOCKET new_socket) {
#define BUF_SIZE 1024
	char buf[BUF_SIZE];
	strcpy_s(buf, BUF_SIZE, "HTTP/1.1 200 OK\r\n");
	strcat_s(buf, BUF_SIZE, "Connection: keep-alive\r\n");
	strcat_s(buf, BUF_SIZE, "Connection: keep-alive\r\n");
	strcat_s(buf, BUF_SIZE, "Content-Type: text/html\r\n");
	strcat_s(buf, BUF_SIZE, "Content-Length: 1024\r\n");
	strcat_s(buf, BUF_SIZE, "\r\n");
	handle_http_request(new_socket);
	char* html = render_html("C:\\Users\\hp\\Documents\\Web Server\\WebServer\\x64\\Debug\\templates\\login.html");
	//strcat_s(message, strlen(html), html);

	//if (send(new_socket, message, (int)strlen(message), 0) < 0)
	//{
	//	puts("Send failed");
	//	return EXIT_FAILURE;
	//}

	return 0;
}

char* render_html(const char* path) {
	FILE* file = NULL;
	int err;

	if ((err = fopen_s(&file, path, "r")) != 0)
	{
		perror("Error: ");
	}

	fseek(file, 0L, SEEK_END);
	long sz = ftell(file);
	char string[1024];
	static char buffer[5000];

	while (fgets(string, sz, file)) //Pega cada linha do texto
	{
		strcat_s(buffer, sz, string);
	}

	return buffer;
}

struct Request handle_http_request(SOCKET new_socket)
{
	const int request_buffer_size = 65536; // 64K
	static char request[request_buffer_size];

	// Read request
	int bytes_recvd = recv(new_socket, request, request_buffer_size - 1, 0);

	if (bytes_recvd < 0) {
		perror("recv");
	}

	struct Request req = {};
	return req;
}

int send_response(int fd, char* header, char* content_type, void* body, int content_length)
{
	const int max_response_size = 262144;
	char response[max_response_size];

	// Build HTTP response and store it in response

	///////////////////
	// IMPLEMENT ME! //
	///////////////////

	// Send it all!
	int rv = send(fd, response, max_response_size, 0);

	if (rv < 0) {
		perror("send");
	}

	return rv;
}
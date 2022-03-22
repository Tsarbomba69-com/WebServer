#include "cweb.h"

SOCKET SERVER_SOCKET, CLIENT_SOCKET = INVALID_SOCKET;


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


int get(const char* uri, SOCKET new_socket, void (*callback)(Request, Response)) {
	char* html = render_template("login.html");

	callback(handle_http_request(new_socket), send_response(
		new_socket,
		"Connection: keep-alive\r\n",
		"Content-Type: text/html\r\n",
		html,
		strlen(html)
	));
	return 0;
}

char* render_template(const char* path) {
	FILE* file = NULL;
	int err;

	if ((err = fopen_s(&file, path, "rb")) != 0 && file == NULL)
	{
		perror("Error: ");
	}
	else {
#define BUF_SIZE 65536
		char string[100];
		static char buffer[BUF_SIZE];
		char chr;

		while (fgets(string, sizeof(string), file)) //Pega cada linha do texto
		{
			strcat_s(buffer, BUF_SIZE, string);
		}

		fclose(file);
		return buffer;
	}
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

	for (int i = 0; i < strlen(request) - 2; i++)
	{
		if (request[i] == '\n' && request[i + 1] == '\n')
		{
			// This makes it possible to separate header fields from the request body.
			request[i + 1] = '|';
		}
	}

	// Separate the request string.
	char* request_line = strtok(request, "\n");
	char* header_fields = strtok(NULL, "|");
	char* body = strtok(NULL, "|");
	struct Request req = {}; 
	extract_request_line_fields(&req, request_line, strlen(request_line));
	return req;
}

void extract_request_line_fields(struct Request* request, char* request_line, size_t length) {
	// Copy the string literal into a local instance.
	char* fields = (char*) malloc(length * sizeof(char));
	strcpy(fields, request_line);
	// Separate the string on spaces for each section.
	char* method = strtok(fields, " ");
	char* uri = strtok(NULL, " ");
	char* http_version = strtok(NULL, "\0");
	request->method = method;
	request->uri = uri;
	request->http_version = http_version;
}

struct Response send_response(SOCKET new_socket, const char* header, const char* content_type, void* body, int content_length)
{
	const int max_response_size = 262144;
	char response[max_response_size];
	char content_len_buffer[10];

	// Build HTTP response and store it in response
	strcpy_s(response, max_response_size, "HTTP/1.1 200 OK\r\n");
	strcat_s(response, max_response_size, header);
	strcat_s(response, max_response_size, content_type);
	strcat_s(response, max_response_size, "Content-Length: ");
	sprintf_s(content_len_buffer, sizeof(content_len_buffer), "%d", content_length);
	strcat_s(response, max_response_size, content_len_buffer);
	strcat_s(response, max_response_size, "\r\n");
	strcat_s(response, max_response_size, "\r\n");
	strcat_s(response, max_response_size, (char*)body);

	// Send it all!
	if (send(new_socket, response, (int)strlen(response), 0) == SOCKET_ERROR) {
		perror("ERROR: send");
		wprintf(L"send failed with error: %d\n", WSAGetLastError());
		closesocket(new_socket);
		WSACleanup();
	}

	struct Response res = {};
	res.body = (char*)malloc(sizeof(body));
	res.content_type = (char*)malloc(sizeof(content_type));
	res.header = (char*)malloc(sizeof(header));
	res.status_code = 200;
	res.content_length = content_length;
	strcpy_s(res.content_type, strlen(content_type) + 1, content_type);
	strcpy_s(res.header, strlen(header) + 1, header);
	return res;
}
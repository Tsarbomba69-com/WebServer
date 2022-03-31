#include "cweb.h"

SOCKET SERVER_SOCKET, CLIENT_SOCKET = INVALID_SOCKET;
Dictionary ROUTES = dict_constructor();


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
	SOCKET new_socket;
	int c = sizeof(struct sockaddr_in);

	//Accept and incoming connection
	puts("Waiting for incoming connections...");

	while ((new_socket = accept(SERVER_SOCKET, (struct sockaddr*)&CLIENT_SOCKET, &c)) != INVALID_SOCKET)
	{
		accept_request(new_socket);
		puts("Connection accepted");
		closesocket(new_socket);
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


int get(const char* uri, void (*callback)(Request*, Response*)) {
	char buffer[1024];
	strcpy_s(buffer, 4, "GET");
	strcat(buffer, uri);
	insert_item(&ROUTES, buffer, callback, sizeof(ROUTES));
	return 0;
}

int post(const char* uri, void (*callback)(Request*, Response*)) {
	char buffer[1024];
	strcpy_s(buffer, 5, "POST");
	strcat(buffer, uri);
	insert_item(&ROUTES, buffer, callback, sizeof(ROUTES));
	return 0;
}

void accept_request(SOCKET new_socket) {
	Request req = handle_http_request(new_socket);
	char* html = render_template("login.html");
	Response res = create_response(
		new_socket,
		"Connection: keep-alive\r\n",
		"Content-Type: text/html\r\n"
	);
	memset(html, 0, sizeof(html));
	char buffer[1024];
	strcpy_s(buffer, strlen(req.method) + 1, req.method);
	strcat(buffer, req.uri);
	void* callback = (void*)get_item(&ROUTES, buffer);
	if (callback)((void(*)(Request*, Response*))callback)(&req, &res);
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

Request handle_http_request(SOCKET new_socket)
{
	const int request_buffer_size = 65536; // 64K
	static char request_string[request_buffer_size];
	char requested[request_buffer_size];

	// Read request
	int bytes_recvd = recv(new_socket, request_string, request_buffer_size - 1, 0);

	if (bytes_recvd < 0) {
		perror("recv");
	}

	strcpy_s(requested, strlen(request_string) + 1, request_string);
	int len = strlen(requested) - 2;
	for (int i = 0; i < len; i++)
	{
		if (requested[i] == '\r' && requested[i + 1] == '\n' && requested[i + 2] == '\r' && requested[i + 3] == '\n')
		{
			// This makes it possible to separate header fields from the request body.
			requested[i] = '|';
			requested[i + 1] = '|';
		}
	}

	// Separate the request string.
	char* request_line = strtok(requested, "\n");
	char* header_fields = strtok(NULL, "|");
	char* body = strtok(NULL, "|");
	body += 2;
	Request req;
	extract_request_line_fields(&req, request_line, strlen(request_line));
	extract_header_fields(&req, header_fields, strlen(header_fields));
	extract_body(&req, body);
	memset(request_line, 0, sizeof(request_line));
	memset(header_fields, 0, sizeof(header_fields));
	memset(body, 0, sizeof(body));
	memset(request_string, 0, sizeof(request_string));
	return req;
}

void extract_request_line_fields(struct Request* request, char* request_line, size_t length) {
	// Copy the string literal into a local instance.
	char* fields = (char*)malloc(length);
	strcpy_s(fields, strlen(request_line) + 1, request_line);
	// Separate the string on spaces for each section.
	char* method = strtok(fields, " ");
	char* uri = strtok(NULL, " ");
	char* http_version = strtok(NULL, "\0");
	request->method = method;
	request->uri = uri;
	request->http_version = http_version;
}

void extract_header_fields(Request* request, char* header_fields, size_t length) {
	char* fields = (char*)malloc(strlen(header_fields));
	strcpy_s(fields, strlen(header_fields) + 1, header_fields);

	// Save each line of the input into a linked-list.
	LinkedList headers = linked_list_constructor();
	char* field = strtok(fields, "\n");
	int i = 0;
	while (field)
	{
		insert(&headers, i++, field, strlen(field) * sizeof(char));
		field = strtok(NULL, "\n");
	}

	//// Initialize the request's headers dictionary.
	request->headers = dict_constructor();
	//// Use the queue to further extract key value pairs.
	for (int i = 0; i < headers.length; i++) {
		char* header = (char*)(iterate(&headers, i))->data;
		char* key = strtok(header, ":");
		char* value = strtok(NULL, "\r");

		if (value)
		{
			// Remove leading white spaces.
			if (value[0] == ' ')
			{
				value++;
			}
			// Push the key value pairs into the request's headers dictionary.
			insert_item(&(request->headers), key, value, sizeof(request->headers));
			// Collect the next field from the queue.
		}
	}

	//// Destroy the linked-list
	//linked_list_destructor(&headers);
}

// Parses the body according to the content type specified in the header fields.
void extract_body(Request* request, char* body) {
	// Check what content type needs to be parsed
	if (strlen(body) == 0) return;
	char* content_type = (char*)get_item(&(request->headers), (char*)"Content-Type");
	char* _body = (char*)calloc(strlen(body), sizeof(char));
	strcpy_s(_body, (strlen(body) + 1) * sizeof(char), body);
	// Initialize the body_fields dictionary.
	request->body = dict_constructor();

	if (content_type) {
		if (strcmp(content_type, "application/x-www-form-urlencoded") == 0) {
			// Save each line of the input into a linked-list.
			LinkedList fields = linked_list_constructor();
			char* field = strtok(_body, "&");
			int i = 0;

			while (field)
			{
				insert(&fields, i++, field, strlen(field) * sizeof(char));
				field = strtok(NULL, "&");
			}

			free(field);

			for (int i = 0; i < fields.length; i++) {
				char* header = (char*)get_list_item(&fields, i);
				char* key = strtok(header, "=");
				char* value = strtok(NULL, "\r");

				if (value)
				{
					// Remove leading white spaces.
					if (value[0] == ' ')
					{
						value++;
					}
					// Push the key value pairs into the request's headers dictionary.
					insert_item(&(request->body), key, value, sizeof(request->body));
					// Collect the next field from the queue.
				}
			}
			//// Destroy the linked-list
			//linked_list_destructor(&fields);
		}
		else
		{
			// Save the data as a single key value pair.
			insert_item(&(request->body), (char*)"data", body, sizeof(strlen(body)));
		}
		// Set the request's body dictionary.

	}

}


Response create_response(SOCKET new_socket, const char* header, const char* content_type)
{
	Response res;
	res.content_type = (char*)malloc(sizeof(strlen(content_type)));
	res.header = (char*)malloc(strlen(header) * sizeof(char)) + 1;
	res.status_code = 200;
	res.socket = new_socket;
	strcpy_s(res.content_type, strlen(content_type) + 1, content_type);
	strcpy_s(res.header, strlen(header) + 1, header);
	return res;
}

void send_response(Response res, const char* body) {
	const int max_response_size = 262144;
	char response[max_response_size] = "";
	char content_len_buffer[10];

	// Build HTTP response and store it in response
	strcpy_s(response, max_response_size, "HTTP/1.1 200 OK\r\n");
	strcat_s(response, max_response_size, res.header);
	strcat_s(response, max_response_size, res.content_type);
	strcat_s(response, max_response_size, "Content-Length: ");
	sprintf_s(content_len_buffer, sizeof(content_len_buffer), "%d", strlen(body));
	strcat_s(response, max_response_size, content_len_buffer);
	strcat_s(response, max_response_size, "\r\n");
	strcat_s(response, max_response_size, "\r\n");
	strcat_s(response, max_response_size, (char*)body);

	// Send it all!
	if (send(res.socket, response, (int)strlen(response), 0) == SOCKET_ERROR) {
		perror("ERROR: send");
		wprintf(L"send failed with error: %d\n", WSAGetLastError());
		closesocket(res.socket);
		WSACleanup();
	}
}
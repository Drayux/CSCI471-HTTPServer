#include "includes.h"
#include "connection.h"
#include "request.h"
#include "response.h"

// Create and bind the listener file descriptor
void getListener(int& listenFD, sockaddr_in* socket_struct) {
    // Create the socket
    listenFD = socket(AF_INET, SOCK_STREAM, 0);
    DEBUG << "Created listener file descriptor with value " << listenFD << ENDL;

    // Bind the socket to the specified address and port
    int port = 5000;

    socket_struct->sin_family = PF_INET;
    socket_struct->sin_addr.s_addr = htonl(INADDR_ANY);
    socket_struct->sin_port = htons(port);

    DEBUG << "Binding listener" << ENDL;
    while (bind(listenFD, (struct sockaddr*) socket_struct, sizeof(*socket_struct))) {
        DEBUG << "Failed to bind port " << port << ENDL;
        socket_struct->sin_port = htons((++port % 5000) + 5000);     // Attempts to connect to a port between 5000 and 9999 (inclusive)

    }
    std::cout << "Using port " << port << std::endl;

    // Set the socket to listen for new connections
    int listenQueueLength = 1;
    DEBUG << "Listening for connections" << ENDL;
    listen(listenFD, listenQueueLength);

}

// Awaits new connections until given signal to exit
// TODO put the SIGINT try-catch in here
void acceptConnection(int& listenFD, int& connectionFD, sockaddr* address) {
    int quit = 0;
    int len = sizeof(address);
    while (!quit) {
        DEBUG << "Awaiting new connection..." << ENDL;
        connectionFD = accept(listenFD, address, (socklen_t*) &len);

        DEBUG << "Connection established with file descriptor " << connectionFD << ENDL;

        //quit = handleConnection(connectionFD);
        handleConnection(connectionFD);
        close(connectionFD);

    }

    close(listenFD);

}

// Handles the data received from the network
void handleConnection(int connection) {
    char line_buf[1024];                    // Line buffer (assuming client will send no more than 1024 bytes at once)
    char* req_buf = new char[1024]{ 0 };    // Complete request buffer (TODO automatically expand this if it becomes full)
    int line_len = 0;
    int req_len = 0;

    do {
        // Clear the line buffer
        memset(line_buf, 0, sizeof(line_buf));

        // Get the next line from the client
        line_len = (int) read(connection, line_buf, sizeof(line_buf));
        DEBUG << "Read " << line_len << " bytes from socket" << ENDL;

        // Append it to the request buffer
        for (int i = 0; i < line_len; i++) {
            *(req_buf + req_len) = line_buf[i];
            req_len++;

        }

        // Break the loop if there are two consecutive CRLF characters
        // This likely to the be how the loop exits
        if (req_len >= 4) {
            char end[] = { *(req_buf + (req_len - 4)), *(req_buf + (req_len - 3)), *(req_buf + (req_len - 2)), *(req_buf + (req_len - 1)) };
            if (!strncmp(end, "\r\n\r\n", 4)) break;
        }
    } while (line_len);

    DEBUG << "TOTAL: " << req_len << " bytes received" << ENDL;

    struct http_request request;
    struct http_response response;

    processRequest(req_buf, req_len, &request);
    buildResponse(&request, &response);

    // DEBUG << "Response status: " << response.status << ENDL;
    DEBUG << "Response length: " << response.content_length << ENDL;

    // Write back to the client
    int status_len = snprintf(NULL, 0, "%s", response.status_line);
    int header_len = 0;
    int data_written = 0;

    // Status line
    write(connection, response.status_line, status_len);
    DEBUG << "Sent " << status_len << " bytes to socket" << ENDL;

    // Headers
    for (int i = 0; i < response.headers.size(); i++) {
        header_len = snprintf(NULL, 0, "%s", response.headers.at(i));

        write(connection, response.headers.at(i), header_len);
        DEBUG << "Sent " << header_len << " bytes to socket" << ENDL;

    }

    write(connection, "\r\n", 2);
    DEBUG << "Sent " << 2 << " bytes to socket" << ENDL;

    // Data
    // (Loop condition could be rewritten in case of data somehow being malformed)
    for (int i = 0; i < response.data.size(); i++) {
        if (response.content_length > 1024) {
            write(connection, response.data.at(i), 1024);
            DEBUG << "Sent " << 1024 << " bytes to socket" << ENDL;

            response.content_length -= 1024;

        } else {
            write(connection, response.data.at(i), response.content_length);
            DEBUG << "Sent " << response.content_length << " bytes to socket" << ENDL;

        }
    }

    write(connection, "\r\n", 2);
    DEBUG << "Sent " << 2 << " bytes to socket" << ENDL;

    // DEBUG << response.status_line << ENDL;
    // DEBUG << response.headers.at(0) << "\n" << ENDL;
    // DEBUG << response.data.at(0) << ENDL;

}

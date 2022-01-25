#include "includes.h"
#include "connection.h"
#include "request.h"
#include "response.h"

// Prepare the response portion of an HTTP connection
void buildResponse(http_request* request, http_response* response) {
    char fnformat_file[] = "file*.html\0\0\0\0\0";
    char fnformat_image[] = "image*.jpg\0\0\0\0\0";

    char* header_line;   // Pointer to current line of header being written
    char* data_line;     // Pointer to current line of data being written
    int data_len = 0;

    bool fn_valid = true;
    int format = -1;      // 0 if file, 1 if image
    int a = 0;            // Index of the filename format string
    int b = 0;            // Index of the requested filename string

    char filepath[BUFSIZE] = { 0 };
    int fp_index = 1;

    FILE* file;

    switch (request->type) {
    case req_type::GET:
        // Offset index if requested file starts with '/' (still a valid request)
        if (request->filename[0] == '/') b = 1;

        // Compare filename strings ('*' is 0 - 9 wildcard)
        while (fn_valid && request->filename[b]) {
            switch (format) {
            case 0:
                if (fnformat_file[a] == '*') {
                    if (!(request->filename[b] >= 48 && request->filename[b] <= 57)) fn_valid = false;

                } else if (!(fnformat_file[a] == request->filename[b])) fn_valid = false;
                break;

            case 1:
                if (fnformat_image[a] == '*') {
                    if (!(request->filename[b] >= 48 && request->filename[b] <= 57)) fn_valid = false;

                } else if (!(fnformat_image[a] == request->filename[b])) fn_valid = false;
                break;

            default:
                if (fnformat_file[a] == request->filename[b]) format = 0;
                else if (fnformat_image[a] == request->filename[b]) format = 1;
                else fn_valid = false;

            }

            a++;
            b++;

        }

        // Throw 404 error if filename is invalid (this would be a 403 in the traditional sense)
        if (!fn_valid) {
            // populateStatus(403, response);
            DEBUG << "Invalid filename requested!" << ENDL;
            populateStatus(404, response);
            return;
        }

        DEBUG << "Valid filename requested!" << ENDL;

        // Convert filename to local filepath
        // Recall that if it started with '/' from the browser, then b will be greater than a
        filepath[0] = '.';
        if (b == a) {
            filepath[1] = '/';
            fp_index = 2;

        }

        for (int i = 0; i < BUFSIZE - fp_index; i++) filepath[i + fp_index] = request->filename[i];

        // Attempt to open the specified file
        file = fopen(filepath, "rb");

        if (!file) {
            DEBUG << "Failed to open requested file" << ENDL;
            populateStatus(404, response);
            return;
        }

        // File exists - Send it to the client!
        populateStatus(200, response);

        // Read the specified file
        // POTENTIAL TODO : This method *could* use a lot of memory; Move the write phase to here instead of pushing it to memory
        // POTENTIAL TODO : If we wanted to request an error page (instead of a hard-coded error message) move this to a helper function
        do {
            data_line = new char[1024]{ 0 };
            response->data.push_back(data_line);

            data_len += fread(data_line, 1, 1024, file);

        } while (!feof(file));

        fclose(file);

        // DEBUG << "File length: " << data_len << ENDL;
        response->content_length = data_len;

        // Header: Content-Type
        header_line = new char[64]{ 0 };
        response->headers.push_back(header_line);
        // Switch here for expandability if need be
        switch (format) {
            case 0:
            strcpy(header_line, "Content-Type: text/html\r\n");
            break;

            case 1:
            strcpy(header_line, "Content-Type: image/jpeg\r\n");
            break;

            default:
            strcpy(header_line, "Content-Type: unknown\r\n");

        }

        // Header: Content-Length
        header_line = new char[64]{ 0 };
        response->headers.push_back(header_line);
        snprintf(header_line, 64, "Content-Length: %d\r\n", data_len);

        return;

    case req_type::HEAD:
    case req_type::POST:
    case req_type::INVALID:
        populateStatus(400, response);

        return;
    }
}

// Populate the response struct with certain values according to the page status
void populateStatus(int code, http_response* response) {
    char* header_line;
    int data_len = 0;

    char* status = new char[64]{ 0 };
    char* status_line = new char[64]{ 0 };
    char* data_line = new char[64]{ 0 };

    response->status = code;
    response->status_line = status;

    switch (code) {
    case 200:
        strcpy(status, "HTTP/1.0 200 OK\r\n");
        DEBUG << "Response status: HTTP/1.0 200 OK" << ENDL;
        delete[] status_line;
        delete[] data_line;
        return;

    case 400:
        strcpy(status_line, "HTTP/1.0 400 Bad Request");
        strcpy(data_line, "Bad Request\r\n");
        data_len = 13;
        break;

    case 403:
        strcpy(status_line, "HTTP/1.0 403 Forbidden");
        strcpy(data_line, "Access Forbidden\r\n");
        data_len = 18;
        break;

    case 404:
        strcpy(status_line, "HTTP/1.0 404 Not Found");
        strcpy(data_line, "File Not Found\r\n");  // If we wanted a call to an error page, read(errorPage) would be put here (if read() were implimented)
        data_len = 16;
        break;

    default:
        strcpy(status_line, "HTTP/1.0 500 Internal Server Error");
        strcpy(data_line, "Internal Server Error\r\n");
        data_len = 23;

    }

    // This section is not called if the status code was 200
    snprintf(status, 64, "%s\r\n", status_line);
    DEBUG << "Response status: " << status_line << ENDL;
    delete[] status_line;

    response->content_length = data_len;
    response->data.push_back(data_line);

    // Header: Content-Type
    header_line = new char[64]{ 0 };
    response->headers.push_back(header_line);
    strcpy(header_line, "Content-Type: text/html\r\n");

    // Header: Content-Length
    header_line = new char[64]{ 0 };
    response->headers.push_back(header_line);
    snprintf(header_line, 64, "Content-Length: %d\r\n", data_len);

}

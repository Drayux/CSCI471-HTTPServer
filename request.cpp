#include "includes.h"
#include "connection.h"
#include "request.h"

// Process the request portion of an HTTP connection
void processRequest(char* req_buf, int req_len, http_request* request) {
    char buf[1024] = { 0 };              // Current line being read
    std::vector<char*> substrs;          // Array of pointers to all non-empty lines
    int strnum = 0;                      // Number of the current line being read
    int placehldr = 0;                   // Index of req_buf at start of line

    // Split the request with respect to newlines
    for (int i = 0; i < req_len; i++) {
        char next = req_buf[i];

        if (next == '\r' && (req_len - i - 1) && req_buf[i + 1] == '\n') {
        //if (next == '5' && (buf_len - i - 1) && req_buf[i + 1] == '6') {
            // DON'T create a new line if the line is empty
            if (i - placehldr) {
                char* line = new char[i - placehldr + 2]{ 0 };
                strcpy(line, buf);
                substrs.push_back(line);

                strnum++;
                memset(buf, 0, sizeof(buf));
            }

            // Since we checked one char in advance for this condition, skip it
            i++;
            placehldr = i + 1;

        } else buf[i - placehldr] = next;
    }

    // Begin parsing the request line
    char type_buf[BUFSIZE] = { 0 };
    char filename[BUFSIZE] = { 0 };
    char version[BUFSIZE] = { 0 };

    // Split it with respect to spaces (if it exists)
    int index = 0;
    char next = 0;
    int segment = 0;
    int placehldr2 = 0;
    if (strnum) {
        do {
            next = *(substrs.at(0) + index);  // NOTE I can't recall if this needs to be `sizeof(char) * i` or just `i`

            // Basically the same process as before, just a lot simpler
            if (next == ' ') {
                segment++;
                placehldr2 = index + 1;

            // Ensures we don't segfault on a malformed request
            } else if (index - placehldr2 >= BUFSIZE) {
                memset(type_buf, 0, sizeof(type_buf));
                break;

            // Specifies which array to write to based on the number of ' ' chars seen
            } else {
                switch(segment) {
                case 0:
                    type_buf[index] = next;
                    break;

                case 1:
                    filename[index - placehldr2] = next;
                    break;

                default:
                    version[index - placehldr2] = next;

                }
            } index++;
        } while (next != 0);
    }

    // Determine request TYPE
    if (!strncmp(type_buf, "GET\0", 4)) request->type = req_type::GET;
    else if (!strncmp(type_buf, "HEAD\0", 5)) request->type = req_type::HEAD;
    else if (!strncmp(type_buf, "POST\0", 5)) request->type = req_type::POST;
    else request->type = req_type::INVALID;

    // Finish up the request struct
    strcpy(request->filename, filename);
    strcpy(request->version, version);     // POTENTIAL TODO : Check that this part of the request is correctly formed

    DEBUG << "Request type: " << type_buf << ENDL;
    DEBUG << "Requested filename: " << filename << ENDL;
    DEBUG << "Requested protocol version: " << version << ENDL;

    // TODO (implimentation not required) Parse headers
    // (This is everything from substrs[1] to substrs[strnum - 1] inclusive)
    DEBUG << "Request made with " << strnum - 1 << " headers" << ENDL;

    // Clean up dynamic memory
    int i = 0;
    for (i = 0; i < strnum; i++) delete[] substrs.at(i);
    DEBUG << "Cleaned up " << i << " strings in the heap" << ENDL;

}

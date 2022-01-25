#ifndef REQUEST_h
#define REQUEST_h

// Datatypes
enum req_type { INVALID = 0, GET, HEAD, POST };
struct http_request {
    req_type type;
    char filename[BUFSIZE] = { 0 };  // We know we can keep this small because we will only accept requests to an explicit set of filenames
    char version[BUFSIZE] = { 0 };
    // POTENTIAL TODO : Support for headers

};

// Functions
void processRequest(char*, int, http_request*);

#endif

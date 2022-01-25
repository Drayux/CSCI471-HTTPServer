#ifndef RESPONSE_h
#define RESPONSE_h

// Datatypes
struct http_response {
    int status;                        // Status code (i.e. 200, 400, 404, etc.)

    char* status_line;
    std::vector<char*> headers;
    std::vector<char*> data;

    int content_length = 0;

    ~http_response() {
        int i = 0;
        int j = 0;
        // Delete all arrays at the headers array
        for (i = 0; i < headers.size(); i++) {
            if (headers.at(i)) delete[] headers.at(i);

        }
        // Delete all arrays at the data array
        for (j = 0; i < data.size(); i++) {
            if (data.at(i)) delete[] data.at(i);

        }
        DEBUG << "Cleaned up " << i + j << " strings in the heap" << ENDL;
    }
};

// Functions
void buildResponse(http_request*, http_response*);
void populateStatus(int, http_response*);

#endif

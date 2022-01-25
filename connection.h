#ifndef CONNECTION_h
#define CONNECTION_h

// Constants
const int BUFSIZE = 16;

// Functions
void getListener(int&, sockaddr_in*);
void acceptConnection(int&, int&, sockaddr*);
void handleConnection(int);

#endif

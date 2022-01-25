#include "includes.h"
#include "connection.h"

// Sets up the sockets and accepts a new connection
int main (int argc, char* argv[]) {
    // Set up logging (I'm not familiar with this library)
    boost::log::add_console_log(std::cout, boost::log::keywords::format = "%Message%");
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);

    // Process command line arguments
    int opt = 0;
    while ((opt = getopt(argc, argv, "v")) != -1) {

        switch (opt) {
        case 'v':
            boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::debug);
            break;

        case ':':
        case '?':
        default:
            std::cout << "Usage: " << argv[0] << " -v" << std::endl;
            exit(-1);

        }
    }

    // Create variables for connection components
    int listenFD = -1;
    int connectionFD = -1;
    struct sockaddr_in socket_struct;

    // Get a connection listener file descriptor
    getListener(listenFD, &socket_struct);

    // Accept the incoming connection
    acceptConnection(listenFD, connectionFD, (struct sockaddr*) &socket_struct);

    return 0;
}

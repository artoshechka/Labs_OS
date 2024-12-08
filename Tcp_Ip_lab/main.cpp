#include "server.h"
#include <iostream>

int main() {
    const int PORT = 8080;

    try {
        Server server(PORT);
        server.run();
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

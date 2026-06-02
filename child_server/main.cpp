#include "ChildServer.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ChildServer <port>\n";
        return 1;
    }
    uint16_t port = static_cast<uint16_t>(std::stoi(argv[1]));
    ChildServer server(port);
    if (!server.Init()) return 1;
    server.Run();
    return 0;
}

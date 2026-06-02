#include "ParentServer.h"
#include "Protocol.h"
#include <iostream>

int main() {
    ParentServer server(Protocol::PARENT_PORT);
    if (!server.Init()) return 1;
    server.Run();
    return 0;
}

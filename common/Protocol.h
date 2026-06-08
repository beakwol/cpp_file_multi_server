#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <cstdint>
#include <vector>

namespace Protocol {
    constexpr uint16_t PARENT_PORT     = 56000;
    constexpr uint16_t CHILD_PORT_BASE = 56001;
    constexpr int      MAX_CHILDREN    = 64;

    constexpr uint32_t HANDSHAKE_HELLO = 0x48454C4F;
    constexpr uint32_t HANDSHAKE_READY = 0x52454459;

    bool SendAll(SOCKET s, const void* buf, int len);
    bool RecvAll(SOCKET s, void* buf, int len);
    bool SendFile(SOCKET s, const std::vector<char>& data);
    bool RecvFile(SOCKET s, std::vector<char>& data);

    bool HandshakeClient(SOCKET s);
    bool HandshakeServer(SOCKET s);
}

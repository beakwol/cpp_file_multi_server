#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include "Protocol.h"

namespace Protocol {

bool SendAll(SOCKET s, const void* buf, int len) {
    const char* ptr = static_cast<const char*>(buf);
    int remaining = len;
    while (remaining > 0) {
        int sent = send(s, ptr, remaining, 0);
        if (sent == SOCKET_ERROR || sent == 0) return false;
        ptr += sent;
        remaining -= sent;
    }
    return true;
}

bool RecvAll(SOCKET s, void* buf, int len) {
    char* ptr = static_cast<char*>(buf);
    int remaining = len;
    while (remaining > 0) {
        int received = recv(s, ptr, remaining, 0);
        if (received == SOCKET_ERROR || received == 0) return false;
        ptr += received;
        remaining -= received;
    }
    return true;
}

bool SendFile(SOCKET s, const std::vector<char>& data) {
    int32_t size = htonl(static_cast<int32_t>(data.size()));
    if (!SendAll(s, &size, sizeof(size))) return false;
    if (!data.empty() && !SendAll(s, data.data(), static_cast<int>(data.size()))) return false;
    return true;
}

bool RecvFile(SOCKET s, std::vector<char>& data) {
    int32_t size = 0;
    if (!RecvAll(s, &size, sizeof(size))) return false;
    size = ntohl(size);
    if (size < 0 || size > 100 * 1024 * 1024) return false;
    data.resize(size);
    if (size > 0 && !RecvAll(s, data.data(), size)) return false;
    return true;
}

bool HandshakeClient(SOCKET s) {
    uint32_t hello = htonl(HANDSHAKE_HELLO);
    if (!SendAll(s, &hello, sizeof(hello))) return false;
    uint32_t reply = 0;
    if (!RecvAll(s, &reply, sizeof(reply))) return false;
    return ntohl(reply) == HANDSHAKE_READY;
}

bool HandshakeServer(SOCKET s) {
    uint32_t hello = 0;
    if (!RecvAll(s, &hello, sizeof(hello))) return false;
    if (ntohl(hello) != HANDSHAKE_HELLO) return false;
    uint32_t ready = htonl(HANDSHAKE_READY);
    return SendAll(s, &ready, sizeof(ready));
}

} // namespace Protocol

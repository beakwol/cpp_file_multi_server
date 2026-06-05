#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "Protocol.h"
#include "SockUtil.h"

namespace Protocol {

bool SendAll(SOCKET s, const void* buf, int len) {
    const char* ptr = static_cast<const char*>(buf);
    int remaining = len;
    while (remaining > 0) {
        int sent = SockUtil::Send(s, ptr, remaining);
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
        int received = SockUtil::Recv(s, ptr, remaining);
        if (received == SOCKET_ERROR || received == 0) return false;
        ptr += received;
        remaining -= received;
    }
    return true;
}

bool SendFile(SOCKET s, const std::vector<char>& data) {
    uint32_t size = SockUtil::HostToNet32(static_cast<uint32_t>(data.size()));
    if (!SendAll(s, &size, sizeof(size))) return false;
    if (!data.empty() && !SendAll(s, data.data(), static_cast<int>(data.size()))) return false;
    return true;
}

bool RecvFile(SOCKET s, std::vector<char>& data) {
    uint32_t size = 0;
    if (!RecvAll(s, &size, sizeof(size))) return false;
    size = SockUtil::NetToHost32(size);
    if (size > 100u * 1024u * 1024u) return false;
    data.resize(size);
    if (size > 0 && !RecvAll(s, data.data(), static_cast<int>(size))) return false;
    return true;
}

bool HandshakeClient(SOCKET s) {
    uint32_t hello = SockUtil::HostToNet32(HANDSHAKE_HELLO);
    if (!SendAll(s, &hello, sizeof(hello))) return false;
    uint32_t reply = 0;
    if (!RecvAll(s, &reply, sizeof(reply))) return false;
    return SockUtil::NetToHost32(reply) == HANDSHAKE_READY;
}

bool HandshakeServer(SOCKET s) {
    uint32_t hello = 0;
    if (!RecvAll(s, &hello, sizeof(hello))) return false;
    if (SockUtil::NetToHost32(hello) != HANDSHAKE_HELLO) return false;
    uint32_t ready = SockUtil::HostToNet32(HANDSHAKE_READY);
    return SendAll(s, &ready, sizeof(ready));
}

} // namespace Protocol

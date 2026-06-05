#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include "SockUtil.h"
#include <iostream>

bool SockUtil::Startup() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "[SockUtil] WSAStartup failed: " << WSAGetLastError() << "\n";
        return false;
    }
    return true;
}

void SockUtil::Cleanup() {
    WSACleanup();
}

int SockUtil::LastError() {
    return WSAGetLastError();
}

SOCKET SockUtil::Create() {
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET)
        std::cerr << "[SockUtil] socket failed: " << WSAGetLastError() << "\n";
    return s;
}

void SockUtil::Close(SOCKET& s) {
    if (s != INVALID_SOCKET) {
        closesocket(s);
        s = INVALID_SOCKET;
    }
}

bool SockUtil::Bind(SOCKET s, uint16_t port) {
    sockaddr_in addr = {};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(port);
    if (bind(s, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "[SockUtil] bind failed (port " << port << "): " << WSAGetLastError() << "\n";
        return false;
    }
    return true;
}

bool SockUtil::Listen(SOCKET s, int backlog) {
    if (listen(s, backlog) == SOCKET_ERROR) {
        std::cerr << "[SockUtil] listen failed: " << WSAGetLastError() << "\n";
        return false;
    }
    return true;
}

SOCKET SockUtil::Accept(SOCKET s) {
    sockaddr_in clientAddr = {};
    int addrLen = sizeof(clientAddr);
    SOCKET client = accept(s, reinterpret_cast<sockaddr*>(&clientAddr), &addrLen);
    if (client == INVALID_SOCKET)
        std::cerr << "[SockUtil] accept failed: " << WSAGetLastError() << "\n";
    return client;
}

bool SockUtil::Connect(SOCKET s, const std::string& host, uint16_t port) {
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);
    inet_pton(AF_INET, host.c_str(), &addr.sin_addr);
    if (connect(s, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "[SockUtil] connect failed (" << host << ":" << port << "): "
                  << WSAGetLastError() << "\n";
        return false;
    }
    return true;
}

int SockUtil::Send(SOCKET s, const char* buf, int len) {
    return send(s, buf, len, 0);
}

int SockUtil::Recv(SOCKET s, char* buf, int len) {
    return recv(s, buf, len, 0);
}

uint16_t SockUtil::HostToNet16(uint16_t v) { return htons(v); }
uint16_t SockUtil::NetToHost16(uint16_t v) { return ntohs(v); }
uint32_t SockUtil::HostToNet32(uint32_t v) { return htonl(v); }
uint32_t SockUtil::NetToHost32(uint32_t v) { return ntohl(v); }

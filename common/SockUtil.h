#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdint>
#include <string>

class SockUtil {
public:
    static bool   Startup();
    static void   Cleanup();
    static int    LastError();

    static SOCKET Create();
    static void   Close(SOCKET& s);

    static bool   Bind(SOCKET s, uint16_t port);
    static bool   Listen(SOCKET s, int backlog = SOMAXCONN);
    static SOCKET Accept(SOCKET s);

    static int    Send(SOCKET s, const char* buf, int len);
    static int    Recv(SOCKET s, char* buf, int len);

    static uint16_t HostToNet16(uint16_t v);
    static uint16_t NetToHost16(uint16_t v);
    static uint32_t HostToNet32(uint32_t v);
    static uint32_t NetToHost32(uint32_t v);

private:
    SockUtil() = delete;
};

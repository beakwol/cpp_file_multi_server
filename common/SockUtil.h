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
    // 초기화 / 정리
    static bool   Startup();
    static void   Cleanup();
    static int    LastError();

    // 소켓 생성 / 종료
    static SOCKET Create();
    static void   Close(SOCKET& s);

    // 서버
    static bool   Bind(SOCKET s, uint16_t port);
    static bool   Listen(SOCKET s, int backlog = SOMAXCONN);
    static SOCKET Accept(SOCKET s);

    // 클라이언트
    static bool   Connect(SOCKET s, const std::string& host, uint16_t port);

    // 송수신 (단순 래핑, 루프 없음)
    static int    Send(SOCKET s, const char* buf, int len);
    static int    Recv(SOCKET s, char* buf, int len);

    // 바이트 오더
    static uint16_t HostToNet16(uint16_t v);
    static uint16_t NetToHost16(uint16_t v);
    static uint32_t HostToNet32(uint32_t v);
    static uint32_t NetToHost32(uint32_t v);

private:
    SockUtil() = delete;
};

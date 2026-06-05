#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdint>
#include <string>

namespace SockUtil {
    // 초기화 / 정리
    bool   Startup();
    void   Cleanup();
    int    LastError();

    // 소켓 생성 / 종료
    SOCKET Create();
    void   Close(SOCKET& s);

    // 서버
    bool   Bind(SOCKET s, uint16_t port);
    bool   Listen(SOCKET s, int backlog = SOMAXCONN);
    SOCKET Accept(SOCKET s);

    // 클라이언트
    bool   Connect(SOCKET s, const std::string& host, uint16_t port);

    // 송수신 (단순 래핑, 루프 없음)
    int    Send(SOCKET s, const char* buf, int len);
    int    Recv(SOCKET s, char* buf, int len);

    // 바이트 오더
    uint16_t HostToNet16(uint16_t v);
    uint16_t NetToHost16(uint16_t v);
    uint32_t HostToNet32(uint32_t v);
    uint32_t NetToHost32(uint32_t v);
}

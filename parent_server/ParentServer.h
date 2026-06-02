#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <windows.h>
#include <string>
#include <cstdint>

class ParentServer {
public:
    explicit ParentServer(uint16_t port = 56000);
    ~ParentServer();

    bool Init();
    void Run();
    void Shutdown();

private:
    uint16_t    m_listenPort;
    SOCKET      m_listenSock = INVALID_SOCKET;
    uint16_t    m_nextChildPort = 56001;

    uint16_t    SpawnChild(uint16_t port);
    std::string GetChildExePath() const;
    uint16_t    AllocatePort();
};

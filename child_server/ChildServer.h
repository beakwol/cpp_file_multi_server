#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <cstdint>
#include <string>
#include <vector>

class ChildServer {
public:
    explicit ChildServer(uint16_t port);
    ~ChildServer();

    bool Init();
    void Run();
    void Shutdown();

private:
    uint16_t m_port;
    SOCKET   m_listenSock = INVALID_SOCKET;

    std::vector<char> ModifyContent(const std::vector<char>& input) const;
    std::string BuildHeader() const;
    std::string BuildFooter() const;
};

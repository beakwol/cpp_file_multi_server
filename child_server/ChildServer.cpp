#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "ChildServer.h"
#include "Protocol.h"
#include "SockUtil.h"
#include <iostream>
#include <ctime>
#include <string>

ChildServer::ChildServer(uint16_t port) : m_port(port) {}

ChildServer::~ChildServer() { Shutdown(); }

bool ChildServer::Init() {
    if (!SockUtil::Startup()) return false;

    m_listenSock = SockUtil::Create();
    if (m_listenSock == INVALID_SOCKET) return false;

    if (!SockUtil::Bind(m_listenSock, m_port) ||
        !SockUtil::Listen(m_listenSock, 1)) {
        return false;
    }

    std::cout << "[Child:" << m_port << "] Listening\n";
    return true;
}

void ChildServer::Run() {
    SOCKET clientSock = SockUtil::Accept(m_listenSock);
    if (clientSock == INVALID_SOCKET) return;

    std::cout << "[Child:" << m_port << "] Client connected. Handshaking...\n";

    if (!Protocol::HandshakeServer(clientSock)) {
        std::cerr << "[Child:" << m_port << "] Handshake failed\n";
        SockUtil::Close(clientSock);
        return;
    }

    std::cout << "[Child:" << m_port << "] Handshake OK. Waiting for file...\n";

    std::vector<char> fileData;
    if (!Protocol::RecvFile(clientSock, fileData)) {
        std::cerr << "[Child:" << m_port << "] Failed to receive file\n";
        SockUtil::Close(clientSock);
        return;
    }

    std::cout << "[Child:" << m_port << "] Received " << fileData.size() << " bytes. Modifying...\n";

    std::vector<char> modified = ModifyContent(fileData);

    if (!Protocol::SendFile(clientSock, modified)) {
        std::cerr << "[Child:" << m_port << "] Failed to send file\n";
    } else {
        std::cout << "[Child:" << m_port << "] Sent " << modified.size() << " bytes\n";
    }

    SockUtil::Close(clientSock);

    // 포트 확인용 대기 (netstat -ano | findstr "5600" 으로 확인 가능)
    constexpr int HOLD_SECONDS = 10;
    std::cout << "[Child:" << m_port << "] Holding for " << HOLD_SECONDS << "s (port verification)...\n";
    for (int i = HOLD_SECONDS; i > 0; --i) {
        std::cout << "[Child:" << m_port << "] Closing in " << i << "s\n";
        Sleep(1000);
    }
    std::cout << "[Child:" << m_port << "] Exiting\n";
}

void ChildServer::Shutdown() {
    SockUtil::Close(m_listenSock);
    SockUtil::Cleanup();
}

std::vector<char> ChildServer::ModifyContent(const std::vector<char>& input) const {
    std::string header = BuildHeader();
    std::string footer = BuildFooter();

    std::vector<char> result;
    result.reserve(header.size() + input.size() + footer.size());
    result.insert(result.end(), header.begin(), header.end());
    result.insert(result.end(), input.begin(),  input.end());
    result.insert(result.end(), footer.begin(), footer.end());
    return result;
}

std::string ChildServer::BuildHeader() const {
    return "=== [MODIFIED BY SERVER] ===\n";
}

std::string ChildServer::BuildFooter() const {
    time_t now = time(nullptr);
    char tsbuf[64];
    struct tm tminfo;
    localtime_s(&tminfo, &now);
    strftime(tsbuf, sizeof(tsbuf), "%Y-%m-%d %H:%M:%S", &tminfo);
    return std::string("\n=== [TIMESTAMP: ") + tsbuf + "] ===\n";
}


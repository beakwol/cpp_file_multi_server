#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <windows.h>
#include "ParentServer.h"
#include "Protocol.h"
#include <iostream>
#include <string>
#include <vector>

ParentServer::ParentServer(uint16_t port) : m_listenPort(port) {}

ParentServer::~ParentServer() { Shutdown(); }

bool ParentServer::Init() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "[Parent] WSAStartup failed: " << WSAGetLastError() << "\n";
        return false;
    }

    m_listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_listenSock == INVALID_SOCKET) {
        std::cerr << "[Parent] socket failed: " << WSAGetLastError() << "\n";
        WSACleanup();
        return false;
    }

    sockaddr_in addr = {};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(m_listenPort);

    if (bind(m_listenSock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "[Parent] bind failed: " << WSAGetLastError() << "\n";
        return false;
    }

    if (listen(m_listenSock, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "[Parent] listen failed: " << WSAGetLastError() << "\n";
        return false;
    }

    std::cout << "[Parent] Listening on port " << m_listenPort << "\n";
    return true;
}

void ParentServer::Run() {
    while (true) {
        sockaddr_in clientAddr = {};
        int addrLen = sizeof(clientAddr);
        SOCKET clientSock = accept(m_listenSock, reinterpret_cast<sockaddr*>(&clientAddr), &addrLen);
        if (clientSock == INVALID_SOCKET) {
            std::cerr << "[Parent] accept failed: " << WSAGetLastError() << "\n";
            break;
        }

        uint16_t childPort = AllocatePort();
        std::cout << "[Parent] Client connected. Spawning child on port " << childPort << "\n";

        if (SpawnChild(childPort) == 0) {
            std::cerr << "[Parent] Failed to spawn child\n";
            closesocket(clientSock);
            continue;
        }

        Sleep(200);

        uint16_t portNet = htons(childPort);
        Protocol::SendAll(clientSock, &portNet, sizeof(portNet));
        closesocket(clientSock);
    }
}

void ParentServer::Shutdown() {
    if (m_listenSock != INVALID_SOCKET) {
        closesocket(m_listenSock);
        m_listenSock = INVALID_SOCKET;
    }
    WSACleanup();
}

uint16_t ParentServer::AllocatePort() {
    return m_nextChildPort++;
}

std::string ParentServer::GetChildExePath() const {
    char buf[MAX_PATH];
    GetModuleFileNameA(NULL, buf, MAX_PATH);
    std::string path(buf);
    auto slash = path.find_last_of("\\/");
    if (slash != std::string::npos)
        path = path.substr(0, slash + 1);
    return path + "ChildServer.exe";
}

uint16_t ParentServer::SpawnChild(uint16_t port) {
    std::string exe = GetChildExePath();
    std::string cmd = "\"" + exe + "\" " + std::to_string(port);
    std::vector<char> cmdBuf(cmd.begin(), cmd.end());
    cmdBuf.push_back('\0');

    STARTUPINFOA        si = { sizeof(si) };
    PROCESS_INFORMATION pi = {};

    BOOL ok = CreateProcessA(
        exe.c_str(),
        cmdBuf.data(),
        NULL, NULL,
        FALSE,
        0,
        NULL, NULL,
        &si, &pi
    );

    if (!ok) {
        std::cerr << "[Parent] CreateProcess failed: " << GetLastError() << "\n";
        return 0;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return port;
}

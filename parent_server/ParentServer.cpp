#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include "ParentServer.h"
#include "Protocol.h"
#include "SockUtil.h"
#include <iostream>
#include <string>
#include <vector>

ParentServer::ParentServer(uint16_t port) : m_listenPort(port) {}

ParentServer::~ParentServer() { Shutdown(); }

bool ParentServer::Init() {
    if (!SockUtil::Startup()) return false;

    m_listenSock = SockUtil::Create();
    if (m_listenSock == INVALID_SOCKET) {
        SockUtil::Cleanup();
        return false;
    }

    if (!SockUtil::Bind(m_listenSock, m_listenPort) ||
        !SockUtil::Listen(m_listenSock)) {
        return false;
    }

    std::cout << "[Parent] Listening on port " << m_listenPort << "\n";
    return true;
}

void ParentServer::Run() {
    while (true) {
        SOCKET clientSock = SockUtil::Accept(m_listenSock);
        if (clientSock == INVALID_SOCKET) break;

        uint16_t childPort = AllocatePort();
        std::cout << "[Parent] Client connected. Spawning child on port " << childPort << "\n";

        if (SpawnChild(childPort) == 0) {
            std::cerr << "[Parent] Failed to spawn child\n";
            SockUtil::Close(clientSock);
            continue;
        }

        Sleep(200);

        uint16_t portNet = SockUtil::HostToNet16(childPort);
        Protocol::SendAll(clientSock, &portNet, sizeof(portNet));
        SockUtil::Close(clientSock);
    }
}

void ParentServer::Shutdown() {
    SockUtil::Close(m_listenSock);
    SockUtil::Cleanup();
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

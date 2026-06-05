#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "Client.h"
#include "Protocol.h"
#include <ws2tcpip.h>
#include <iostream>
#include <fstream>
#include <string>

class MyNetUtil {
public:
    static inline bool Startup() {
        WSADATA wsa;
        return (WSAStartup(MAKEWORD(2, 2), &wsa) == 0);
    }
    static inline void Cleanup() { WSACleanup(); }
    static inline SOCKET Create() {
        return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    }
    static inline void Close(SOCKET& s) {
        if (s != INVALID_SOCKET) { closesocket(s); s = INVALID_SOCKET; }
    }
    static inline uint16_t NetToHost16(uint16_t v) { return ntohs(v); }
    static inline bool MyConnect(SOCKET s, const std::string& host, uint16_t port) {
        sockaddr_in addr = {};
        addr.sin_family = AF_INET;
        addr.sin_port   = htons(port);
        inet_pton(AF_INET, host.c_str(), &addr.sin_addr);
        if (::connect(s, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR)
            return false;
        return true;
    }
};

Client::Client() {}
Client::~Client() { Cleanup(); }

bool Client::Init() {
    return MyNetUtil::Startup();
}

void Client::Cleanup() {
    MyNetUtil::Cleanup();
}

bool Client::Run(const std::string& host, uint16_t parentPort, const std::string& filePath) {
    if (!Init()) return false;

    uint16_t childPort = 0;
    if (!GetChildPort(host, parentPort, childPort)) {
        std::cerr << "[Client] Failed to get child port\n";
        return false;
    }

    std::cout << "[Client] Redirected to child server on port " << childPort << "\n";

    if (!TransferFile(host, childPort, filePath)) {
        std::cerr << "[Client] File transfer failed\n";
        return false;
    }

    return true;
}

bool Client::GetChildPort(const std::string& host, uint16_t parentPort, uint16_t& outChildPort) {
    SOCKET sock = MyNetUtil::Create();
    if (sock == INVALID_SOCKET) return false;

    if (!MyNetUtil::MyConnect(sock, host, parentPort)) {
        MyNetUtil::Close(sock);
        return false;
    }

    std::cout << "[Client] Connected to parent (" << host << ":" << parentPort << ")\n";

    uint16_t portNet = 0;
    bool ok = Protocol::RecvAll(sock, &portNet, sizeof(portNet));
    MyNetUtil::Close(sock);

    if (!ok) return false;
    outChildPort = MyNetUtil::NetToHost16(portNet);
    return true;
}

bool Client::TransferFile(const std::string& host, uint16_t childPort, const std::string& filePath) {
    std::ifstream fin(filePath, std::ios::binary);
    if (!fin) {
        std::cerr << "[Client] Cannot open file: " << filePath << "\n";
        return false;
    }
    std::vector<char> fileData((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
    fin.close();

    SOCKET sock = MyNetUtil::Create();
    if (sock == INVALID_SOCKET) return false;

    if (!MyNetUtil::MyConnect(sock, host, childPort)) {
        MyNetUtil::Close(sock);
        return false;
    }

    std::cout << "[Client] Performing handshake...\n";
    if (!Protocol::HandshakeClient(sock)) {
        std::cerr << "[Client] Handshake failed\n";
        MyNetUtil::Close(sock);
        return false;
    }
    std::cout << "[Client] Handshake OK. Sending " << fileData.size() << " bytes...\n";

    if (!Protocol::SendFile(sock, fileData)) {
        MyNetUtil::Close(sock);
        return false;
    }

    std::vector<char> result;
    if (!Protocol::RecvFile(sock, result)) {
        MyNetUtil::Close(sock);
        return false;
    }

    MyNetUtil::Close(sock);
    return SaveResult(filePath, result);
}

bool Client::SaveResult(const std::string& originalPath, const std::vector<char>& data) {
    std::string outPath = originalPath;
    auto dot = outPath.find_last_of('.');
    if (dot != std::string::npos)
        outPath = outPath.substr(0, dot) + "_modified" + outPath.substr(dot);
    else
        outPath += "_modified";

    std::ofstream fout(outPath, std::ios::binary);
    if (!fout) {
        std::cerr << "[Client] Cannot write output file: " << outPath << "\n";
        return false;
    }
    fout.write(data.data(), data.size());
    std::cout << "[Client] Saved modified file: " << outPath << "\n";
    return true;
}

#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <string>
#include <vector>
#include <cstdint>

class Client {
public:
    Client();
    ~Client();

    bool Run(const std::string& host, uint16_t parentPort, const std::string& filePath);

private:
    bool Init();
    void Cleanup();

    bool GetChildPort(const std::string& host, uint16_t parentPort, uint16_t& outChildPort);
    bool TransferFile(const std::string& host, uint16_t childPort, const std::string& filePath);
    bool SaveResult(const std::string& originalPath, const std::vector<char>& data);
};

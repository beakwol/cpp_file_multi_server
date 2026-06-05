#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <commdlg.h>
#include "Client.h"
#include "Protocol.h"
#include <iostream>
#include <string>

static std::string OpenFileDialog() {
    char filename[MAX_PATH] = {};
    OPENFILENAMEA ofn      = { sizeof(ofn) };
    ofn.lpstrFile          = filename;
    ofn.nMaxFile           = MAX_PATH;
    ofn.lpstrFilter        = "All Files\0*.*\0Text Files\0*.TXT\0";
    ofn.lpstrTitle         = "전송할 파일 선택";
    ofn.Flags              = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    if (GetOpenFileNameA(&ofn))
        return std::string(filename);
    return "";
}

int main(int argc, char* argv[]) {
    std::string host     = "127.0.0.1";
    std::string filePath;

    if (argc >= 2) host     = argv[1];
    if (argc >= 3) filePath = argv[2];

    if (filePath.empty()) {
        std::cout << "[Client] 파일을 선택하세요 (창이 열립니다)...\n";
        filePath = OpenFileDialog();
    }

    if (filePath.empty()) {
        std::cout << "[Client] 파일 경로를 직접 입력하세요: ";
        std::getline(std::cin, filePath);
    }

    if (filePath.empty()) {
        std::cerr << "[Client] 파일이 선택되지 않았습니다.\n";
        return 1;
    }

    std::cout << "[Client] 선택된 파일: " << filePath << "\n";

    Client client;
    if (!client.Run(host, Protocol::PARENT_PORT, filePath))
        return 1;
    return 0;
}

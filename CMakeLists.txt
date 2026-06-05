cmake_minimum_required(VERSION 3.20)
project(MultiServer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(protocol STATIC common/Protocol.cpp)
target_include_directories(protocol PUBLIC common)
target_link_libraries(protocol PUBLIC ws2_32)

add_executable(ParentServer
    parent_server/ParentServer.cpp
    parent_server/parent_main.cpp
)
target_include_directories(ParentServer PRIVATE parent_server)
target_link_libraries(ParentServer PRIVATE protocol)

add_executable(ChildServer
    child_server/ChildServer.cpp
    child_server/child_main.cpp
)
target_include_directories(ChildServer PRIVATE child_server)
target_link_libraries(ChildServer PRIVATE protocol)

add_executable(Client
    client/Client.cpp
    client/client_main.cpp
)
target_include_directories(Client PRIVATE client)
target_link_libraries(Client PRIVATE protocol comdlg32)

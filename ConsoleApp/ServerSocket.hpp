//
// Created by 이태형 on 2023-12-02.
//

#pragma once
#include "Socket.hpp"
class ServerSocket
{
public:
    ServerSocket(int port);
    Socket* Accept();
    ~ServerSocket();
private:
    int sock;
};
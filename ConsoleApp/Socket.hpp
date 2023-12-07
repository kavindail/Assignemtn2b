//
// Created by 이태형 on 2023-12-02.
//

#ifndef ASSIGNMENT2B_SOCKET_HPP
#define ASSIGNMENT2B_SOCKET_HPP

#endif //ASSIGNMENT2B_SOCKET_HPP


#pragma once
class Socket
{
public:
    Socket(int sock);
    char* getRequest();
    void sendResponse(char* res);
    ~Socket();
private:
    int sock;
};

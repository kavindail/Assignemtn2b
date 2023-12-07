//
// Created by 이태형 on 2023-12-02.
//

#include "Socket.hpp"
#include <sys/socket.h>
#include <sys/types.h>
#include <resolv.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

Socket::Socket(int sock)
{
    this->sock = sock;
}
char* Socket::getRequest()
{
    int rval;
    char *buf = new char[1024];

    if ((rval = read(sock, buf, 1024)) < 0){
        perror("reading socket");
    }else  {
        printf("%s\n",buf);
    }

    return buf;
}
void Socket::sendResponse(char *res){
    int rval;

    if ((rval = write(sock, res, strlen(res))) < 0){
        perror("writing socket");
    }else  {
        printf("%s\n",res);
    }

    return;
}
Socket::~Socket()
{
}
//
// Created by 이태형 on 2023-12-02.
//

#include "Socket.hpp"
#include "ServerSocket.hpp"
#include <stddef.h>
int main() {
    ServerSocket *ss = new ServerSocket(8888);
    if (ss != NULL) {
        Socket *cs = ss->Accept();
        char *req = cs->getRequest();
        char *res = new char(50);
        cs->sendResponse(res);
    }
}

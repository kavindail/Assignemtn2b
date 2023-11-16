//
// Created by 이태형 on 2023-11-15.
//

#include <iostream>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include <cstring>
#include "UploadServlet.cpp"

#ifndef ASSIGNMENT2B_UPLOADSERVERTHREAD_HPP
#define ASSIGNMENT2B_UPLOADSERVERTHREAD_HPP


class UploadServerThread {
private:
    int clientSocket;

    static void *handleRequests(void *arg) {
        auto *threadInstance = (UploadServerThread *)arg;
        threadInstance->run();
        delete threadInstance;
        return nullptr;
    }

public:
    UploadServerThread(int clientSocket) : clientSocket(clientSocket) {}

    void start();

    void run();
};
#endif //ASSIGNMENT2B_UPLOADSERVERTHREAD_HPP

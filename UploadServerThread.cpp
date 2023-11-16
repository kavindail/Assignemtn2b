//
// Created by 이태형 on 2023-11-15.
//

#include "UploadServerThread.hpp"

void UploadServerThread::run(){
    char buffer[2048];
    read(clientSocket, buffer, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    UploadServlet uploadServlet;

    if (strstr(buffer, "GET") != NULL) {
        std::cout << "GET request received" << std::endl;
        uploadServlet.doGet(clientSocket);
    }

    if (strstr(buffer, "POST") != NULL) {
        std::cout << "POST request received" << std::endl;
        uploadServlet.doPost(clientSocket);
        std::cout << "File upload handled" << std::endl;
    }

    close(clientSocket);
}

void UploadServerThread::start() {
    pthread_t thread;
    if (pthread_create(&thread, nullptr, UploadServerThread::handleRequests, this) != 0) {
        perror("Error creating thread");
    }
    pthread_detach(thread);
}
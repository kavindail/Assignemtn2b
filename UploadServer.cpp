#include <iostream>
#include <netinet/in.h>
#include <unistd.h>

// Assuming UploadServerThread is a class you have defined to handle individual client requests.
#include "UploadServerThread.cpp"

class UploadServer {
public:
    static void startServer(int port) {
        int serverSocket;
        sockaddr_in serverAddress;

        // Create socket
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket < 0) {
            perror("Error opening socket");
            exit(1);
        }

        // Initialize socket structure
        memset(&serverAddress, 0, sizeof(serverAddress));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        serverAddress.sin_port = htons(port);

        // Bind the host address
        if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
            perror("Error binding socket");
            close(serverSocket);
            exit(1);
        }

        // Start listening for the clients
        if (listen(serverSocket, 5) < 0) {
            perror("Error listening on socket");
            close(serverSocket);
            exit(1);
        }

        std::cout << "Server started on port " << port << std::endl;

        // Server loop
        while (true) {
            sockaddr_in cli_addr;
            socklen_t clilen = sizeof(cli_addr);

            // Accept actual connection from the client
            int newSocket = accept(serverSocket, (struct sockaddr *)&cli_addr, &clilen);
            if (newSocket < 0) {
                perror("Error on accept");
                continue;
            }
            std::cout << "Connection accepted." << std::endl;

            // Delegate to UploadServerThread to handle the client
            UploadServerThread *serverThread = new UploadServerThread(newSocket);
            serverThread->start();
        }

        // Close the server socket
        close(serverSocket);
    }
};

int main(int argc, char *argv[]) {
    // Start the server on port 8082
    UploadServer::startServer(8084);
    return 0;
}
